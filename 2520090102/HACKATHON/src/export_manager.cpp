#include "export_manager.h"
#include <fstream>
#include <sstream>

namespace pfs {

std::string ExportManager::escapeCsvField(const std::string& field) {
    bool needsQuotes = false;
    for (char c : field) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r') {
            needsQuotes = true;
            break;
        }
    }

    if (!needsQuotes) {
        return field;
    }

    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }
    escaped += "\"";
    return escaped;
}

bool ExportManager::exportToCsv(
    const std::string& filePath,
    const std::vector<SearchResult>& results,
    std::string& outErrorMessage
) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        outErrorMessage = "Unable to create or write to file: " + filePath;
        return false;
    }

    // Write UTF-8 Byte Order Mark (BOM) for compatibility with Excel
    const unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    file.write(reinterpret_cast<const char*>(bom), sizeof(bom));

    // Write CSV Header
    file << "\"Result ID\",\"Filename\",\"File Path\",\"Match Type\",\"Line Number\",\"Matching Text\",\"Status\"\r\n";

    for (const auto& res : results) {
        std::string lineStr = (res.lineNumber > 0) ? std::to_string(res.lineNumber) : "-";

        file << escapeCsvField(std::to_string(res.id)) << ","
             << escapeCsvField(res.fileName) << ","
             << escapeCsvField(res.filePath) << ","
             << escapeCsvField(matchTypeToString(res.matchType)) << ","
             << escapeCsvField(lineStr) << ","
             << escapeCsvField(res.matchedText) << ","
             << escapeCsvField(res.status) << "\r\n";
    }

    if (file.fail()) {
        outErrorMessage = "Error writing CSV data to file: " + filePath;
        return false;
    }

    outErrorMessage.clear();
    return true;
}

} // namespace pfs
