#include "thread_pool.h"

namespace pfs {

ThreadPool::ThreadPool(size_t threads) {
    if (threads == 0) {
        threads = 1;
    }
    m_workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i) {
        m_workers.emplace_back(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop.store(true, std::memory_order_release);
    }
    m_cvTaskAvailable.notify_all();

    for (std::thread& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    m_workers.clear();
}

void ThreadPool::enqueueTask(std::function<void()> task) {
    if (!task) return;
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        if (m_stop.load(std::memory_order_acquire)) {
            return;
        }
        m_tasks.push(std::move(task));
    }
    m_cvTaskAvailable.notify_one();
}

void ThreadPool::cancel() {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        // Clear all pending tasks
        std::queue<std::function<void()>> empty;
        std::swap(m_tasks, empty);
        m_stop.store(true, std::memory_order_release);
    }
    m_cvTaskAvailable.notify_all();
    m_cvTasksFinished.notify_all();
}

void ThreadPool::waitAll() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_cvTasksFinished.wait(lock, [this]() {
        return m_tasks.empty() && m_activeWorkers.load(std::memory_order_relaxed) == 0;
    });
}

void ThreadPool::resize(size_t threads) {
    shutdown();
    m_stop.store(false, std::memory_order_release);
    if (threads == 0) threads = 1;
    m_workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i) {
        m_workers.emplace_back(&ThreadPool::workerLoop, this);
    }
}

size_t ThreadPool::pendingTaskCount() const {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    return m_tasks.size();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cvTaskAvailable.wait(lock, [this]() {
                return m_stop.load(std::memory_order_acquire) || !m_tasks.empty();
            });

            if (m_stop.load(std::memory_order_acquire) && m_tasks.empty()) {
                return;
            }

            if (!m_tasks.empty()) {
                task = std::move(m_tasks.front());
                m_tasks.pop();
                m_activeWorkers.fetch_add(1, std::memory_order_relaxed);
            }
        }

        if (task) {
            try {
                task();
            } catch (...) {
                // Catch any uncaught task exceptions to prevent worker thread abort
            }
            m_activeWorkers.fetch_sub(1, std::memory_order_relaxed);

            // Check if all work is completed
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                if (m_tasks.empty() && m_activeWorkers.load(std::memory_order_relaxed) == 0) {
                    m_cvTasksFinished.notify_all();
                }
            }
        }
    }
}

} // namespace pfs
