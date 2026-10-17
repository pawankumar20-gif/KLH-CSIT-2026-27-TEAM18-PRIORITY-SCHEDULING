#include "search_engine.h"
#include <iostream>
#include <system_error>

namespace pfs {

SearchEngine::SearchEngine()
    : m_lastProgressNotification(std::chrono::steady_clock::now()) {
}

SearchEngine::~SearchEngine() {
    cancel();
    if (m_coordinatorThread.joinable()) {
        m_coordinatorThread.join();
    }
}

bool SearchEngine::validateOptions(const SearchOptions& options, std::string& outErrorMessage) {
    if (options.rootPath.empty()) {
        outErrorMessage = "Search directory cannot be empty.";
        return false;
    }

    std::error_code ec;
    std::filesystem::path p(options.rootPath);
    if (!std::filesystem::exists(p, ec) || ec) {
        outErrorMessage = "Directory does not exist: " + options.rootPath;
        return false;
    }

    if (!std::filesystem::is_directory(p, ec) || ec) {
        outErrorMessage = "Specified path is not a directory: " + options.rootPath;
        return false;
    }

    if (options.isRegex && !options.pattern.empty()) {
        std::string regexErr;
        if (!SearchWorker::isValidRegex(options.pattern, options.caseSensitive, regexErr)) {
            outErrorMessage = "Invalid regular expression: " + regexErr;
            return false;
        }
    }

    if (options.threadCount == 0) {
        outErrorMessage = "Thread count must be at least 1.";
        return false;
    }

    outErrorMessage.clear();
    return true;
}

void SearchEngine::clear() {
    std::lock_guard<std::mutex> lock(m_resultsMutex);
    m_results.clear();
    m_filesDiscovered.store(0, std::memory_order_relaxed);
    m_filesScanned.store(0, std::memory_order_relaxed);
    m_matchesFound.store(0, std::memory_order_relaxed);
    m_nextResultId.store(1, std::memory_order_relaxed);
}

SearchStats SearchEngine::getStats() const {
    SearchStats stats;
    stats.filesDiscovered = m_filesDiscovered.load(std::memory_order_relaxed);
    stats.filesScanned = m_filesScanned.load(std::memory_order_relaxed);
    stats.matchesFound = m_matchesFound.load(std::memory_order_relaxed);
    stats.activeThreads = m_threadPool ? m_threadPool->activeWorkerCount() : 0;

    auto now = m_isRunning.load(std::memory_order_relaxed) ? 
               std::chrono::steady_clock::now() : m_endTime;
    stats.elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();

    if (stats.filesDiscovered > 0) {
        stats.progressPercentage = (static_cast<double>(stats.filesScanned) / 
                                   static_cast<double>(stats.filesDiscovered)) * 100.0;
        if (stats.progressPercentage > 100.0) stats.progressPercentage = 100.0;
    } else {
        stats.progressPercentage = m_isRunning.load(std::memory_order_relaxed) ? 0.0 : 100.0;
    }

    return stats;
}

std::vector<SearchResult> SearchEngine::getAllResults() const {
    std::lock_guard<std::mutex> lock(m_resultsMutex);
    return m_results;
}

bool SearchEngine::startAsync(const SearchOptions& options) {
    std::string errMsg;
    if (!validateOptions(options, errMsg)) {
        Logger::instance().error("Configuration error: " + errMsg);
        return false;
    }

    if (m_isRunning.load(std::memory_order_acquire)) {
        Logger::instance().warn("Search already in progress. Ignoring start request.");
        return false;
    }

    if (m_coordinatorThread.joinable()) {
        m_coordinatorThread.join();
    }

    clear();
    m_cancelRequested.store(false, std::memory_order_release);
    m_isRunning.store(true, std::memory_order_release);
    m_startTime = std::chrono::steady_clock::now();

    m_coordinatorThread = std::thread(&SearchEngine::executeSearch, this, options);
    return true;
}

bool SearchEngine::startSync(const SearchOptions& options) {
    std::string errMsg;
    if (!validateOptions(options, errMsg)) {
        Logger::instance().error("Configuration error: " + errMsg);
        return false;
    }

    if (m_isRunning.load(std::memory_order_acquire)) {
        return false;
    }

    clear();
    m_cancelRequested.store(false, std::memory_order_release);
    m_isRunning.store(true, std::memory_order_release);
    m_startTime = std::chrono::steady_clock::now();

    executeSearch(options);
    return true;
}

void SearchEngine::cancel() {
    m_cancelRequested.store(true, std::memory_order_release);
    if (m_threadPool) {
        m_threadPool->cancel();
    }
}

void SearchEngine::notifyProgress() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastProgressNotification).count();
    if (elapsed >= 40) { // Throttle notifications to ~25fps to keep UI butter smooth
        m_lastProgressNotification = now;
        if (m_progressCallback) {
            m_progressCallback(getStats());
        }
    }
}

void SearchEngine::processFileBatch(const std::vector<std::filesystem::path>& batch, const SearchOptions& options) {
    SearchWorker worker(options);
    std::vector<SearchResult> batchMatches;

    for (const auto& path : batch) {
        if (m_cancelRequested.load(std::memory_order_relaxed)) {
            break;
        }

        std::string fileError;
        auto matches = worker.searchFile(path, m_cancelRequested, fileError);

        if (!fileError.empty()) {
            Logger::instance().warn("File [" + path.filename().string() + "]: " + fileError);
        }

        if (!matches.empty()) {
            for (auto& m : matches) {
                batchMatches.push_back(std::move(m));
            }
        }

        m_filesScanned.fetch_add(1, std::memory_order_relaxed);
    }

    if (!batchMatches.empty() && !m_cancelRequested.load(std::memory_order_relaxed)) {
        {
            std::lock_guard<std::mutex> lock(m_resultsMutex);
            for (auto& match : batchMatches) {
                match.id = m_nextResultId.fetch_add(1, std::memory_order_relaxed);
                m_results.push_back(match);
                if (m_resultCallback) {
                    m_resultCallback(match);
                }
            }
            m_matchesFound.fetch_add(batchMatches.size(), std::memory_order_relaxed);
        }

        if (m_batchResultCallback) {
            m_batchResultCallback(batchMatches);
        }
    }

    notifyProgress();
}

void SearchEngine::executeSearch(const SearchOptions& options) {
    Logger::instance().info("Initializing search engine with " + std::to_string(options.threadCount) + " worker threads.");
    Logger::instance().info("Target path: " + options.rootPath);
    Logger::instance().info("Search pattern: \"" + options.pattern + "\" (Regex: " + 
                           (options.isRegex ? "Yes" : "No") + ", Case-sensitive: " + 
                           (options.caseSensitive ? "Yes" : "No") + ", Recursive: " + 
                           (options.recursive ? "Yes" : "No") + ")");

    m_threadPool = std::make_unique<ThreadPool>(options.threadCount);

    constexpr size_t BATCH_SIZE = 25;
    std::vector<std::filesystem::path> currentBatch;
    currentBatch.reserve(BATCH_SIZE);

    try {
        auto dirOpts = std::filesystem::directory_options::skip_permission_denied;

        std::error_code ecIter;
        if (options.recursive) {
            std::filesystem::recursive_directory_iterator iter(
                options.rootPath, dirOpts, ecIter
            );
            std::filesystem::recursive_directory_iterator end;

            while (iter != end && !m_cancelRequested.load(std::memory_order_relaxed)) {
                std::error_code ec;
                const auto& entry = *iter;

                // Protect against symlink loops or inaccessible files
                bool isRegular = entry.is_regular_file(ec);
                if (!ec && isRegular) {
                    m_filesDiscovered.fetch_add(1, std::memory_order_relaxed);
                    currentBatch.push_back(entry.path());

                    if (currentBatch.size() >= BATCH_SIZE) {
                        auto batchCopy = std::move(currentBatch);
                        currentBatch.clear();
                        currentBatch.reserve(BATCH_SIZE);

                        m_threadPool->enqueueTask([this, batch = std::move(batchCopy), options]() {
                            processFileBatch(batch, options);
                        });
                    }
                }

                iter.increment(ec);
                if (ec) {
                    Logger::instance().warn("Directory traversal error: " + ec.message());
                }
            }
        } else {
            std::filesystem::directory_iterator iter(
                options.rootPath, dirOpts, ecIter
            );
            std::filesystem::directory_iterator end;

            while (iter != end && !m_cancelRequested.load(std::memory_order_relaxed)) {
                std::error_code ec;
                const auto& entry = *iter;

                bool isRegular = entry.is_regular_file(ec);
                if (!ec && isRegular) {
                    m_filesDiscovered.fetch_add(1, std::memory_order_relaxed);
                    currentBatch.push_back(entry.path());

                    if (currentBatch.size() >= BATCH_SIZE) {
                        auto batchCopy = std::move(currentBatch);
                        currentBatch.clear();
                        currentBatch.reserve(BATCH_SIZE);

                        m_threadPool->enqueueTask([this, batch = std::move(batchCopy), options]() {
                            processFileBatch(batch, options);
                        });
                    }
                }

                iter.increment(ec);
                if (ec) {
                    Logger::instance().warn("Directory traversal error: " + ec.message());
                }
            }
        }

        // Enqueue remaining files in last batch
        if (!currentBatch.empty() && !m_cancelRequested.load(std::memory_order_relaxed)) {
            m_threadPool->enqueueTask([this, batch = std::move(currentBatch), options]() {
                processFileBatch(batch, options);
            });
        }

    } catch (const std::exception& e) {
        Logger::instance().error("Exception during directory scan: " + std::string(e.what()));
    }

    Logger::instance().info("Discovery completed. Found " + std::to_string(m_filesDiscovered.load()) + " files. Awaiting worker completion...");

    // Wait for all workers to finish their batches
    if (m_threadPool) {
        m_threadPool->waitAll();
    }

    m_endTime = std::chrono::steady_clock::now();
    m_isRunning.store(false, std::memory_order_release);

    auto finalStats = getStats();
    bool cancelled = m_cancelRequested.load(std::memory_order_acquire);

    if (cancelled) {
        Logger::instance().warn("Search was cancelled by user. Scanned " + 
                                std::to_string(finalStats.filesScanned) + " files, found " + 
                                std::to_string(finalStats.matchesFound) + " matches in " + 
                                std::to_string(finalStats.elapsedMilliseconds) + " ms.");
    } else {
        Logger::instance().success("Search completed successfully! Scanned " + 
                                  std::to_string(finalStats.filesScanned) + " files, found " + 
                                  std::to_string(finalStats.matchesFound) + " matches in " + 
                                  std::to_string(finalStats.elapsedMilliseconds) + " ms.");
    }

    // Final guaranteed notifications
    if (m_progressCallback) {
        m_progressCallback(finalStats);
    }
    if (m_finishedCallback) {
        m_finishedCallback(cancelled, finalStats);
    }
}

} // namespace pfs
