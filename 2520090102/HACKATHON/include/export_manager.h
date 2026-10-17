#pragma once

#include "search_result.h"
#include <string>
#include <vector>

namespace pfs {

class ExportManager {
public:
    // Export search results to a CSV file.
    // Writes UTF-8 BOM for Microsoft Excel compatibility.
    static bool exportToCsv(
        const std::string& filePath,
        const std::vector<SearchResult>& results,
        std::string& outErrorMessage
    );

    // Escape a single CSV field following RFC 4180
    static std::string escapeCsvField(const std::string& field);
};

} // namespace pfs
