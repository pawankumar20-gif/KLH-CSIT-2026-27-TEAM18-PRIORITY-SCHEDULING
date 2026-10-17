#include "search_engine.h"
#include <iostream>
#include <cassert>
#include <filesystem>

static std::string getTestDataDir() {
    if (std::filesystem::exists("test_data")) return "test_data";
    if (std::filesystem::exists("../test_data")) return "../test_data";
    return "../../test_data";
}

void testFilenameSearch() {
    std::cout << "[RUNNING] testFilenameSearch...\n";
    pfs::SearchEngine engine;
    pfs::SearchOptions opt;
    opt.rootPath = getTestDataDir();
    opt.pattern = "file1.txt";
    opt.mode = pfs::SearchMode::FilenamesOnly;
    opt.recursive = true;
    opt.caseSensitive = false;
    opt.threadCount = 4;

    bool ok = engine.startSync(opt);
    assert(ok);

    auto results = engine.getAllResults();
    assert(results.size() >= 1);
    assert(results[0].fileName == "file1.txt");
    assert(results[0].matchType == pfs::MatchType::Filename);
    std::cout << "[PASSED] testFilenameSearch (found " << results.size() << " match)\n";
}

void testTextPatternSearch() {
    std::cout << "[RUNNING] testTextPatternSearch...\n";
    pfs::SearchEngine engine;
    pfs::SearchOptions opt;
    opt.rootPath = getTestDataDir();
    opt.pattern = "AlphaBetaGamma";
    opt.mode = pfs::SearchMode::ContentOnly;
    opt.recursive = true;
    opt.caseSensitive = true;
    opt.threadCount = 4;

    bool ok = engine.startSync(opt);
    assert(ok);

    auto results = engine.getAllResults();
    assert(results.size() == 1);
    assert(results[0].fileName == "file1.txt");
    assert(results[0].matchType == pfs::MatchType::Content);
    assert(results[0].lineNumber > 0);
    std::cout << "[PASSED] testTextPatternSearch\n";
}

void testCaseSensitivity() {
    std::cout << "[RUNNING] testCaseSensitivity...\n";
    pfs::SearchEngine engine;
    pfs::SearchOptions opt;
    opt.rootPath = getTestDataDir();
    opt.pattern = "ALPHABETAGAMMA";
    opt.mode = pfs::SearchMode::ContentOnly;
    opt.recursive = true;
    opt.caseSensitive = true;
    opt.threadCount = 2;

    // Case sensitive with mismatched case should find 0 matches
    engine.startSync(opt);
    assert(engine.getAllResults().empty());

    // Case insensitive should find 1 match
    opt.caseSensitive = false;
    engine.startSync(opt);
    assert(engine.getAllResults().size() == 1);
    std::cout << "[PASSED] testCaseSensitivity\n";
}

void testRecursiveVsNonRecursive() {
    std::cout << "[RUNNING] testRecursiveVsNonRecursive...\n";
    pfs::SearchEngine engine;
    pfs::SearchOptions opt;
    opt.rootPath = getTestDataDir();
    opt.pattern = "DeepAlpha";
    opt.mode = pfs::SearchMode::ContentOnly;
    opt.caseSensitive = true;
    opt.threadCount = 2;

    // Non-recursive: deep_file.txt in subfolder should NOT be found
    opt.recursive = false;
    engine.startSync(opt);
    assert(engine.getAllResults().empty());

    // Recursive: deep_file.txt MUST be found
    opt.recursive = true;
    engine.startSync(opt);
    assert(engine.getAllResults().size() == 1);
    std::cout << "[PASSED] testRecursiveVsNonRecursive\n";
}

void testRegexPatternMatching() {
    std::cout << "[RUNNING] testRegexPatternMatching...\n";
    pfs::SearchEngine engine;
    pfs::SearchOptions opt;
    opt.rootPath = getTestDataDir();
    opt.pattern = R"(\[ERROR\]\s+Connection)";
    opt.mode = pfs::SearchMode::ContentOnly;
    opt.recursive = true;
    opt.isRegex = true;
    opt.caseSensitive = false;
    opt.threadCount = 2;

    bool ok = engine.startSync(opt);
    assert(ok);
    auto results = engine.getAllResults();
    assert(results.size() == 1);
    assert(results[0].fileName == "file2.log");

    // Invalid regex validation
    opt.pattern = "[unclosed-bracket";
    std::string errMsg;
    bool valid = pfs::SearchEngine::validateOptions(opt, errMsg);
    assert(!valid);
    assert(!errMsg.empty());
    std::cout << "[PASSED] testRegexPatternMatching\n";
}

void testInvalidDirectoryAndEmptySearch() {
    std::cout << "[RUNNING] testInvalidDirectoryAndEmptySearch...\n";
    pfs::SearchOptions opt;
    opt.rootPath = "non_existent_directory_12345";
    opt.pattern = "test";

    std::string errMsg;
    bool valid = pfs::SearchEngine::validateOptions(opt, errMsg);
    assert(!valid);
    assert(!errMsg.empty());

    // Empty search pattern (allowed: should discover files without failing)
    opt.rootPath = getTestDataDir();
    opt.pattern = "";
    opt.mode = pfs::SearchMode::FilenamesOnly;
    valid = pfs::SearchEngine::validateOptions(opt, errMsg);
    assert(valid);
    std::cout << "[PASSED] testInvalidDirectoryAndEmptySearch\n";
}

void testBinaryFileHandling() {
    std::cout << "[RUNNING] testBinaryFileHandling...\n";
    pfs::SearchEngine engine;
    pfs::SearchOptions opt;
    opt.rootPath = getTestDataDir();
    opt.pattern = "PATTERN";
    opt.mode = pfs::SearchMode::ContentOnly;
    opt.recursive = false;
    opt.caseSensitive = true;
    opt.threadCount = 2;

    engine.startSync(opt);
    // binary.bin contains bytes 'P','A','T','T','E','R','N' but binary files must be skipped for content search
    auto results = engine.getAllResults();
    assert(results.empty());
    std::cout << "[PASSED] testBinaryFileHandling\n";
}

int main() {
    std::cout << "=== Running SearchEngine Unit Tests ===\n";
    testFilenameSearch();
    testTextPatternSearch();
    testCaseSensitivity();
    testRecursiveVsNonRecursive();
    testRegexPatternMatching();
    testInvalidDirectoryAndEmptySearch();
    testBinaryFileHandling();
    std::cout << "=== All SearchEngine Tests Passed! ===\n";
    return 0;
}
