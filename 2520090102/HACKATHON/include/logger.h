#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include <chrono>

namespace pfs {

enum class LogLevel {
    Info,
    Warning,
    Error,
    Success
};

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;

    std::string formattedTime() const;
    std::string levelString() const;
    std::string toFormattedString() const;
};

class Logger {
public:
    using LogCallback = std::function<void(const LogEntry&)>;

    static Logger& instance();

    void log(LogLevel level, const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void success(const std::string& message);

    void setCallback(LogCallback callback);
    void clear();

    std::vector<LogEntry> entries() const;
    bool exportToFile(const std::string& filePath) const;

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    mutable std::mutex m_mutex;
    std::vector<LogEntry> m_entries;
    LogCallback m_callback;
};

} // namespace pfs
