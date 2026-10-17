#include "search_engine.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <thread>
#include <chrono>

static std::string getTestDataDir() {
    if (std::filesystem::exists("test_data")) return "test_data";
    if (std::filesystem::exists("../test_data")) return "../test_data";
    return "../../test_data";
}

void testSearchCancellation() {
    std::cout << "[RUNNING] testSearchCancellation...\n";
    pfs::SearchEngine engine;
    pfs::SearchOptions opt;
    opt.rootPath = getTestDataDir();
    opt.pattern = "Alpha";
    opt.mode = pfs::SearchMode::Both;
    opt.recursive = true;
    opt.threadCount = 4;

    bool started = engine.startAsync(opt);
    assert(started);

    // Promptly cancel
    engine.cancel();

    // Give coordinator thread moment to join
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assert(!engine.isRunning());
    assert(engine.isCancelled());
    std::cout << "[PASSED] testSearchCancellation\n";
}

int main() {
    std::cout << "=== Running Cancellation Unit Tests ===\n";
    testSearchCancellation();
    std::cout << "=== All Cancellation Tests Passed! ===\n";
    return 0;
}
