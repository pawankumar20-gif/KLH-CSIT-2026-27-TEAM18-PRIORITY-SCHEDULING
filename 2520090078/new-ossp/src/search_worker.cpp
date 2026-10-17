#include "search_worker.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace pfs {

static std::string toLower(std::string_view sv) {
    std::string s(sv);
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

SearchWorker::SearchWorker(const SearchOptions& options)
    : m_options(options) {
    if (m_options.isRegex && !m_options.pattern.empty()) {
        try {
            auto flags = std::regex::ECMAScript;
            if (!m_options.caseSensitive) {
                flags |= std::regex::icase;
            }
            m_regex = std::regex(m_options.pattern, flags);
        } catch (...) {
            m_regex = std::nullopt;
        }
    } else if (!m_options.caseSensitive) {
        m_lowercasePattern = toLower(m_options.pattern);
    }
}

bool SearchWorker::isValidRegex(const std::string& pattern, bool caseSensitive, std::string& outErrorMessage) {
    try {
        auto flags = std::regex::ECMAScript;
        if (!caseSensitive) {
            flags |= std::regex::icase;
        }
        std::regex reg(pattern, flags);
        (void)reg;
        outErrorMessage.clear();
        return true;
    } catch (const std::regex_error& e) {
        outErrorMessage = e.what();
        return false;
    }
}

bool SearchWorker::containsSubstring(
    std::string_view haystack,
    std::string_view needle,
    bool caseSensitive
) {
    if (needle.empty()) return true;
    if (haystack.size() < needle.size()) return false;

    if (caseSensitive) {
        return haystack.find(needle) != std::string_view::npos;
    }

    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](char ch1, char ch2) {
            return std::tolower(static_cast<unsigned char>(ch1)) ==
                   std::tolower(static_cast<unsigned char>(ch2));
        }
    );
    return it != haystack.end();
}

bool SearchWorker::matchString(std::string_view text) const {
    if (m_options.pattern.empty()) {
        return true;
    }

    if (m_options.isRegex && m_regex.has_value()) {
        std::string s(text);
        return std::regex_search(s, *m_regex);
    }

    return containsSubstring(text, m_options.pattern, m_options.caseSensitive);
}

bool SearchWorker::isBinaryFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    constexpr size_t bufferSize = 2048;
    char buffer[bufferSize];
    file.read(buffer, bufferSize);
    std::streamsize bytesRead = file.gcount();

    if (bytesRead == 0) {
        return false; // Empty file is not considered binary
    }

    size_t nonPrintableCount = 0;
    for (std::streamsize i = 0; i < bytesRead; ++i) {
        unsigned char c = static_cast<unsigned char>(buffer[i]);
        if (c == 0) {
            // Null byte found -> definitely binary
            return true;
        }
        if (c < 32 && c != '\t' && c != '\r' && c != '\n') {
            nonPrintableCount++;
        }
    }

    // If more than 15% non-printable ASCII, consider it binary
    return (nonPrintableCount * 100 / static_cast<size_t>(bytesRead)) > 15;
}

std::vector<SearchResult> SearchWorker::searchFilename(const std::filesystem::path& path) {
    std::vector<SearchResult> results;
    std::string filename = path.filename().string();
    if (matchString(filename)) {
        SearchResult res;
        res.fileName = filename;
        res.filePath = path.string();
        res.matchType = MatchType::Filename;
        res.lineNumber = -1;
        res.matchedText = filename;
        res.status = "Filename match";
        results.push_back(std::move(res));
    }
    return results;
}

std::vector<SearchResult> SearchWorker::searchFileContent(
    const std::filesystem::path& path,
    const std::atomic<bool>& cancelRequested,
    std::string& outError
) {
    std::vector<SearchResult> results;

    std::error_code ec;
    auto sz = std::filesystem::file_size(path, ec);
    if (ec) {
        outError = "Cannot check file size: " + ec.message();
        return results;
    }
    if (sz == 0) {
        return results; // Empty file has no content matches
    }
    if (sz > m_options.maxFileSize) {
        outError = "File skipped: exceeds maximum file size limit (" + 
                   std::to_string(m_options.maxFileSize / (1024 * 1024)) + " MB)";
        return results;
    }

    if (isBinaryFile(path)) {
        return results; // Skip binary files cleanly
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        outError = "Failed to open file for reading (permission denied or locked)";
        return results;
    }

    std::string line;
    int lineNumber = 1;
    const std::string filename = path.filename().string();
    const std::string fullPath = path.string();

    while (std::getline(file, line)) {
        if (cancelRequested.load(std::memory_order_relaxed)) {
            break;
        }

        if (matchString(line)) {
            SearchResult res;
            res.fileName = filename;
            res.filePath = fullPath;
            res.matchType = MatchType::Content;
            res.lineNumber = lineNumber;

            // Trim leading whitespace for clean display
            size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
            if (firstNonSpace != std::string::npos) {
                res.matchedText = line.substr(firstNonSpace);
            } else {
                res.matchedText = line;
            }

            // Cap extremely long lines for display
            if (res.matchedText.size() > 500) {
                res.matchedText = res.matchedText.substr(0, 497) + "...";
            }

            res.status = "Line " + std::to_string(lineNumber) + " match";
            results.push_back(std::move(res));
        }

        lineNumber++;
    }

    return results;
}

std::vector<SearchResult> SearchWorker::searchFile(
    const std::filesystem::path& path,
    const std::atomic<bool>& cancelRequested,
    std::string& outError
) {
    std::vector<SearchResult> allMatches;

    if (cancelRequested.load(std::memory_order_relaxed)) {
        return allMatches;
    }

    // 1. Check Filename match if requested
    if (m_options.mode == SearchMode::FilenamesOnly || m_options.mode == SearchMode::Both) {
        auto filenameMatches = searchFilename(path);
        for (auto& match : filenameMatches) {
            allMatches.push_back(std::move(match));
        }
    }

    if (cancelRequested.load(std::memory_order_relaxed)) {
        return allMatches;
    }

    // 2. Check Content match if requested
    if (m_options.mode == SearchMode::ContentOnly || m_options.mode == SearchMode::Both) {
        auto contentMatches = searchFileContent(path, cancelRequested, outError);
        for (auto& match : contentMatches) {
            allMatches.push_back(std::move(match));
        }
    }

    return allMatches;
}

} // namespace pfs
