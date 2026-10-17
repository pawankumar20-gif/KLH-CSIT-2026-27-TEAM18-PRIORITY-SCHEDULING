#include "logger.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace pfs {

std::string LogEntry::formattedTime() const {
    auto timeT = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tmBuffer{};
#if defined(_WIN32)
    localtime_s(&tmBuffer, &timeT);
#else
    localtime_r(&timeT, &tmBuffer);
#endif
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << tmBuffer.tm_hour << ":"
        << std::setw(2) << tmBuffer.tm_min << ":"
        << std::setw(2) << tmBuffer.tm_sec;
    return oss.str();
}

std::string LogEntry::levelString() const {
    switch (level) {
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Success: return "SUCCESS";
        default:                return "DEBUG";
    }
}

std::string LogEntry::toFormattedString() const {
    return "[" + formattedTime() + "] [" + levelString() + "] " + message;
}

Logger& Logger::instance() {
    static Logger s_instance;
    return s_instance;
}

void Logger::log(LogLevel level, const std::string& message) {
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.message = message;

    LogCallback cbCopy;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_entries.push_back(entry);
        cbCopy = m_callback;
    }

    if (cbCopy) {
        cbCopy(entry);
    }
}

void Logger::info(const std::string& message) {
    log(LogLevel::Info, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::Error, message);
}

void Logger::success(const std::string& message) {
    log(LogLevel::Success, message);
}

void Logger::setCallback(LogCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback = std::move(callback);
}

void Logger::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_entries.clear();
}

std::vector<LogEntry> Logger::entries() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entries;
}

bool Logger::exportToFile(const std::string& filePath) const {
    std::vector<LogEntry> entriesCopy;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        entriesCopy = m_entries;
    }

    std::ofstream out(filePath);
    if (!out.is_open()) {
        return false;
    }

    out << "Parallel File Search - Activity Log Export\n";
    out << "==========================================\n\n";

    for (const auto& entry : entriesCopy) {
        out << entry.toFormattedString() << "\n";
    }

    return true;
}

} // namespace pfs
