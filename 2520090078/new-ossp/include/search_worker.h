#pragma once

#include "search_result.h"
#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <atomic>
#include <optional>

namespace pfs {

class SearchWorker {
public:
    explicit SearchWorker(const SearchOptions& options);

    // Search a single file, returns matches found
    std::vector<SearchResult> searchFile(
        const std::filesystem::path& path,
        const std::atomic<bool>& cancelRequested,
        std::string& outError
    );

    // Binary file check
    static bool isBinaryFile(const std::filesystem::path& path);

    // Test regex validity
    static bool isValidRegex(const std::string& pattern, bool caseSensitive, std::string& outErrorMessage);

    // Case-insensitive substring search helper
    static bool containsSubstring(
        std::string_view haystack,
        std::string_view needle,
        bool caseSensitive
    );

private:
    std::vector<SearchResult> searchFilename(const std::filesystem::path& path);
    std::vector<SearchResult> searchFileContent(
        const std::filesystem::path& path,
        const std::atomic<bool>& cancelRequested,
        std::string& outError
    );

    bool matchString(std::string_view text) const;

    SearchOptions m_options;
    std::optional<std::regex> m_regex;
    std::string m_lowercasePattern;
};

} // namespace pfs
