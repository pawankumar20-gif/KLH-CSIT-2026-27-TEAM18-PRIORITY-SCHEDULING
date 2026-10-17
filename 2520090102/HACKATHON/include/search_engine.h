#pragma once

#include "search_result.h"
#include "thread_pool.h"
#include "search_worker.h"
#include "logger.h"
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <chrono>

namespace pfs {

class SearchEngine {
public:
    using ResultCallback = std::function<void(const SearchResult&)>;
    using BatchResultCallback = std::function<void(const std::vector<SearchResult>&)>;
    using ProgressCallback = std::function<void(const SearchStats&)>;
    using FinishedCallback = std::function<void(bool cancelled, const SearchStats&)>;

    SearchEngine();
    ~SearchEngine();

    // Prevent copying
    SearchEngine(const SearchEngine&) = delete;
    SearchEngine& operator=(const SearchEngine&) = delete;

    // Callbacks
    void setResultCallback(ResultCallback cb) { m_resultCallback = std::move(cb); }
    void setBatchResultCallback(BatchResultCallback cb) { m_batchResultCallback = std::move(cb); }
    void setProgressCallback(ProgressCallback cb) { m_progressCallback = std::move(cb); }
    void setFinishedCallback(FinishedCallback cb) { m_finishedCallback = std::move(cb); }

    // Start search synchronously (blocks until completed/cancelled - great for CLI & tests)
    bool startSync(const SearchOptions& options);

    // Start search asynchronously (spawns coordinator thread)
    bool startAsync(const SearchOptions& options);

    // Cancel current search
    void cancel();

    // Check status
    bool isRunning() const { return m_isRunning.load(std::memory_order_acquire); }
    bool isCancelled() const { return m_cancelRequested.load(std::memory_order_acquire); }

    // Get snapshot of statistics
    SearchStats getStats() const;

    // Get all accumulated results
    std::vector<SearchResult> getAllResults() const;

    // Clear accumulated results and reset counters
    void clear();

    // Validation
    static bool validateOptions(const SearchOptions& options, std::string& outErrorMessage);

private:
    void executeSearch(const SearchOptions& options);
    void discoverAndSchedule(const SearchOptions& options);
    void processFileBatch(const std::vector<std::filesystem::path>& batch, const SearchOptions& options);
    void notifyProgress();

    std::unique_ptr<ThreadPool> m_threadPool;
    std::thread m_coordinatorThread;

    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_cancelRequested{false};

    std::atomic<size_t> m_filesDiscovered{0};
    std::atomic<size_t> m_filesScanned{0};
    std::atomic<size_t> m_matchesFound{0};
    std::atomic<size_t> m_nextResultId{1};

    std::chrono::steady_clock::time_point m_startTime;
    std::chrono::steady_clock::time_point m_endTime;

    mutable std::mutex m_resultsMutex;
    std::vector<SearchResult> m_results;

    ResultCallback m_resultCallback;
    BatchResultCallback m_batchResultCallback;
    ProgressCallback m_progressCallback;
    FinishedCallback m_finishedCallback;

    std::chrono::steady_clock::time_point m_lastProgressNotification;
};

} // namespace pfs
