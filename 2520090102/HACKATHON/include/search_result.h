#pragma once

#include <string>
#include <cstddef>
#include <chrono>

namespace pfs {

enum class MatchType {
    Filename,
    Content
};

inline const char* matchTypeToString(MatchType type) {
    switch (type) {
        case MatchType::Filename: return "Filename";
        case MatchType::Content: return "Content";
        default: return "Unknown";
    }
}

struct SearchResult {
    size_t id{0};
    std::string fileName;
    std::string filePath;
    MatchType matchType{MatchType::Filename};
    int lineNumber{-1};
    std::string matchedText;
    std::string status{"Match Found"};
    std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};
};

enum class SearchMode {
    FilenamesOnly,
    ContentOnly,
    Both
};

struct SearchOptions {
    std::string rootPath;
    std::string pattern;
    SearchMode mode{SearchMode::Both};
    bool recursive{true};
    bool caseSensitive{false};
    bool isRegex{false};
    unsigned int threadCount{4};
    size_t maxFileSize{50 * 1024 * 1024}; // Skip files larger than 50MB by default
};

struct SearchStats {
    size_t filesDiscovered{0};
    size_t filesScanned{0};
    size_t matchesFound{0};
    int activeThreads{0};
    int64_t elapsedMilliseconds{0};
    double progressPercentage{0.0};
};

} // namespace pfs
