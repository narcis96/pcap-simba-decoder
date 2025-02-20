#ifndef SAFE_VECTOR_H
#define SAFE_VECTOR_H

#include <vector>
#include <mutex>
#include <condition_variable>

template <typename T>
class SafeVector {
public:
    explicit SafeVector(size_t capacity) {
        vector_.reserve(capacity);
    }

    bool push(T&& value) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            vector_.push_back(std::move(value));
        }
        cv_.notify_one();
        return true;
    }

    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return offset_ < vector_.size() || done_; });
        
        if (offset_ == vector_.size()) return false; // If empty and done flag is set
        
        value = std::move(vector_[offset_++]);
        
        cv_.notify_one();
        return true;
    }

    void setDone() {
        std::lock_guard<std::mutex> lock(mutex_);
        done_ = true;
        cv_.notify_all();
    }

private:
    std::vector<T> vector_;
    bool done_ = false;
    size_t offset_ = 0;
    std::mutex mutex_;
    std::condition_variable cv_;    
};

#endif // SAFE_VECTOR_H