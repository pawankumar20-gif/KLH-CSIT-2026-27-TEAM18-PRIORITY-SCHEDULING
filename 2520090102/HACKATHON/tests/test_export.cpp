#include "export_manager.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

void testCsvEscaping() {
    std::cout << "[RUNNING] testCsvEscaping...\n";
    assert(pfs::ExportManager::escapeCsvField("simple") == "simple");
    assert(pfs::ExportManager::escapeCsvField("comma,separated") == "\"comma,separated\"");
    assert(pfs::ExportManager::escapeCsvField("quote \"inside\"") == "\"quote \"\"inside\"\"\"");
    assert(pfs::ExportManager::escapeCsvField("line1\nline2") == "\"line1\nline2\"");
    std::cout << "[PASSED] testCsvEscaping\n";
}

void testCsvExportToFile() {
    std::cout << "[RUNNING] testCsvExportToFile...\n";
    std::vector<pfs::SearchResult> results;
    
    pfs::SearchResult r1;
    r1.id = 1;
    r1.fileName = "test.txt";
    r1.filePath = "C:/test/test.txt";
    r1.matchType = pfs::MatchType::Filename;
    r1.lineNumber = -1;
    r1.matchedText = "test.txt";
    r1.status = "Filename match";
    results.push_back(r1);

    pfs::SearchResult r2;
    r2.id = 2;
    r2.fileName = "data,with,comma.txt";
    r2.filePath = "C:/test/data,with,comma.txt";
    r2.matchType = pfs::MatchType::Content;
    r2.lineNumber = 42;
    r2.matchedText = "Match with \"quotes\" and commas, here";
    r2.status = "Line 42 match";
    results.push_back(r2);

    std::string testCsvPath = "test_export_output.csv";
    std::string errMsg;
    bool ok = pfs::ExportManager::exportToCsv(testCsvPath, results, errMsg);
    assert(ok);
    assert(errMsg.empty());

    // Verify file exists and has UTF-8 BOM
    std::ifstream in(testCsvPath, std::ios::binary);
    assert(in.is_open());

    unsigned char bom[3] = {0};
    in.read(reinterpret_cast<char*>(bom), 3);
    assert(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF);

    std::string line;
    std::getline(in, line); // Read remaining of header
    assert(line.find("Result ID") != std::string::npos);

    in.close();
    std::filesystem::remove(testCsvPath);

    std::cout << "[PASSED] testCsvExportToFile\n";
}

int main() {
    std::cout << "=== Running CSV Export Unit Tests ===\n";
    testCsvEscaping();
    testCsvExportToFile();
    std::cout << "=== All CSV Export Tests Passed! ===\n";
    return 0;
}
