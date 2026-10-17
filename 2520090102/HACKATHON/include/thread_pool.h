#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <memory>
#include <stdexcept>

namespace pfs {

class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    // Prevent copying
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Enqueue a task returning a future
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (m_stop.load(std::memory_order_acquire)) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_cvTaskAvailable.notify_one();
        return res;
    }

    // Direct void task enqueue (low overhead)
    void enqueueTask(std::function<void()> task);

    // Stop accepting new tasks and cancel pending tasks in the queue
    void cancel();

    // Wait until queue is empty and all active workers have finished
    void waitAll();

    // Resize or reinitialize pool
    void resize(size_t threads);

    // Status queries
    int activeWorkerCount() const { return m_activeWorkers.load(std::memory_order_relaxed); }
    size_t totalWorkerCount() const { return m_workers.size(); }
    size_t pendingTaskCount() const;
    bool isStopped() const { return m_stop.load(std::memory_order_acquire); }

private:
    void workerLoop();
    void shutdown();

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;

    mutable std::mutex m_queueMutex;
    std::condition_variable m_cvTaskAvailable;
    std::condition_variable m_cvTasksFinished;

    std::atomic<bool> m_stop{false};
    std::atomic<int> m_activeWorkers{0};
    std::atomic<bool> m_isPaused{false};
};

} // namespace pfs
