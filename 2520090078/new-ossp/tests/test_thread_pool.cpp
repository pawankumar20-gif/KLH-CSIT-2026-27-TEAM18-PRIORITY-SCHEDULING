#include "thread_pool.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <atomic>
#include <chrono>

void testBasicTaskExecution() {
    std::cout << "[RUNNING] testBasicTaskExecution...\n";
    pfs::ThreadPool pool(4);
    auto fut = pool.enqueue([]() {
        return 42;
    });
    assert(fut.get() == 42);
    std::cout << "[PASSED] testBasicTaskExecution\n";
}

void testHighConcurrencyTasks() {
    std::cout << "[RUNNING] testHighConcurrencyTasks...\n";
    pfs::ThreadPool pool(8);
    constexpr int TASK_COUNT = 500;
    std::atomic<int> counter{0};

    for (int i = 0; i < TASK_COUNT; ++i) {
        pool.enqueueTask([&counter]() {
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }

    pool.waitAll();
    assert(counter.load() == TASK_COUNT);
    assert(pool.activeWorkerCount() == 0);
    std::cout << "[PASSED] testHighConcurrencyTasks\n";
}

void testThreadPoolShutdownAndResize() {
    std::cout << "[RUNNING] testThreadPoolShutdownAndResize...\n";
    pfs::ThreadPool pool(2);
    assert(pool.totalWorkerCount() == 2);

    pool.resize(6);
    assert(pool.totalWorkerCount() == 6);

    std::atomic<int> sum{0};
    for (int i = 0; i < 50; ++i) {
        pool.enqueueTask([&sum]() {
            sum.fetch_add(1, std::memory_order_relaxed);
        });
    }
    pool.waitAll();
    assert(sum.load() == 50);
    std::cout << "[PASSED] testThreadPoolShutdownAndResize\n";
}

int main() {
    std::cout << "=== Running ThreadPool Unit Tests ===\n";
    testBasicTaskExecution();
    testHighConcurrencyTasks();
    testThreadPoolShutdownAndResize();
    std::cout << "=== All ThreadPool Tests Passed! ===\n";
    return 0;
}
