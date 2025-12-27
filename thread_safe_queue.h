#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadSafeQueue {
public:
    using Task = std::function<void()>;
    
    void push(Task task) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(task));
        cv_.notify_one();
    }
    
    bool tryPop(Task& task) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        task = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    
    void waitAndPop(Task& task) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        task = std::move(queue_.front());
        queue_.pop();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
private:
    mutable std::mutex mutex_;
    std::queue<Task> queue_;
    std::condition_variable cv_;
};

#endif