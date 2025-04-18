#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) : stop(false)
{
    std::cerr << "Starting " << numThreads << " threads" << std::endl;
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(
            [this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        // Wait until there is a task or the pool is stopping.
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    // Execute the task.
                    task();
                }
            }
        );
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
        worker.join();
}
