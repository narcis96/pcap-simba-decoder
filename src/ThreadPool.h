#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <type_traits> // For std::invoke_result_t

class ThreadPool {
public:
    // Create a ThreadPool with the specified number of worker threads.
    explicit ThreadPool(size_t numThreads);

    // Destructor will join all threads.
    ~ThreadPool();

    // Enqueue a task into the thread pool. The task is any callable object.
    // Returns a future holding the result of the task.
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<std::invoke_result_t<F, Args...>>;

private:
    // Vector holding all worker threads.
    std::vector<std::thread> workers;
    // Queue for tasks.
    std::queue<std::function<void()>> tasks;

    // Synchronization primitives.
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// Template definition must be in the header.
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<std::invoke_result_t<F, Args...>>
{
    using return_type = std::invoke_result_t<F, Args...>;
    
    // Wrap the function and its arguments into a packaged_task.
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

#endif // THREADPOOL_H
