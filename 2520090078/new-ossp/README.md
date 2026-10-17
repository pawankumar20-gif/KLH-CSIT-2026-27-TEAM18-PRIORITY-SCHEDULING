# Parallel File Search Using Threads

![Application Interface](docs/search_results_screenshot.png)

A high-performance, modern C++ desktop application for concurrent file and content pattern searching. Built with **C++17**, **Qt 6**, and standard library concurrency primitives (`std::thread`, `std::mutex`, `std::atomic`, `std::condition_variable`).

---

## Key Features

- **Genuine Multithreaded Search Engine:** Reusable, thread-safe `ThreadPool` with work batching that dynamically distributes search tasks across multiple CPU cores.
- **Three Search Modes:**
  1. *Both Filenames & File Contents*
  2. *Filenames Only*
  3. *File Contents Only*
- **Rich Pattern Matching:**
  - Standard substring search (case-sensitive or case-insensitive)
  - Regular expressions (`std::regex` ECMAScript syntax) with syntax validation
- **Smart File Filtering:**
  - Automated binary file detection (checks null bytes and non-printable byte density)
  - Incremental line-by-line reading via `std::ifstream` (constant memory usage per file)
  - Maximum file size safety guard (skips oversized files > 50 MB)
  - Directory symlink recursion protection and permission-denied recovery
- **Real-Time Performance Dashboard:**
  - Discovered files counter
  - Scanned files counter
  - Matches found counter
  - Active worker threads indicator
  - Monotonic elapsed time tracker
  - Accurate progress percentage bar based on actual work completed
- **Interactive Results Presentation:**
  - Sortable results table (`#`, `Filename`, `Path`, `Type`, `Line`, `Matching Text`, `Status`)
  - Instant in-memory quick-filter search bar
  - Double-click to open files or folders with the system's default application
  - Right-click context menu (Open File, Open Containing Folder, Copy Path, Copy Text)
  - RFC 4180 CSV Export with UTF-8 BOM encoding for seamless Microsoft Excel compatibility
- **Leveled Activity Logging:**
  - Timestamped, color-coded logging (`[INFO]`, `[SUCCESS]`, `[WARN]`, `[ERROR]`)
  - Export log to text file or clear with a single click
- **Graceful Cancellation:**
  - Responsive, thread-safe cancellation that immediately halts pending tasks and cleanly joins worker threads without crashing or resource leaks.

---

## Technology Stack

- **Language:** C++17
- **Concurrency:** C++ Standard Library (`std::thread`, `std::mutex`, `std::atomic`, `std::condition_variable`, `std::future`)
- **File System:** `std::filesystem`, `std::ifstream`
- **Pattern Matching:** Standard string algorithms and `std::regex`
- **GUI Framework:** Qt 6 (Qt Widgets)
- **Build System:** CMake (>= 3.16) and Ninja
- **Testing:** Automated C++ unit tests managed by CTest

---

## Project Structure

```
ParallelFileSearch/
├── CMakeLists.txt              # Root build configuration
├── README.md                   # Project overview & build guide
├── .gitignore                  # Git ignore rules
├── include/                    # Header files
│   ├── export_manager.h        # RFC 4180 CSV export manager
│   ├── logger.h                # Leveled, thread-safe logger
│   ├── search_controller.h     # Qt mediator connecting GUI and engine
│   ├── search_engine.h         # Multithreaded search orchestrator
│   ├── search_result.h         # SearchResult & SearchOptions data structs
│   ├── search_worker.h         # Per-file pattern matching & binary detection
│   └── thread_pool.h           # Reusable worker thread pool
├── src/                        # Implementation files
│   ├── export_manager.cpp
│   ├── logger.cpp
│   ├── main.cpp                # Application entry point
│   ├── main_window.cpp         # Modern dark theme Qt Widgets GUI
│   ├── search_controller.cpp
│   ├── search_engine.cpp
│   ├── search_worker.cpp
│   └── thread_pool.cpp
├── ui/
│   └── main_window.h           # MainWindow header definition
├── tests/                      # Automated unit test suite
│   ├── CMakeLists.txt
│   ├── test_cancellation.cpp   # Search cancellation test
│   ├── test_export.cpp         # CSV escaping and BOM test
│   ├── test_search_engine.cpp  # Pattern, regex, and directory tests
│   └── test_thread_pool.cpp    # Concurrency and future tests
├── test_data/                  # Sample test directory & files
│   ├── file1.txt
│   ├── file2.log
│   ├── empty.txt
│   ├── unicode_sample.txt
│   ├── binary.bin
│   └── subdir/
│       ├── nested.txt
│       ├── code.cpp
│       └── deep/
│           └── deep_file.txt
└── docs/                       # Documentation & Screenshots
    ├── architecture.md         # Detailed architectural specification
    ├── user_guide.md           # User guide and interface walkthrough
    ├── app_screenshot.png      # Clean initial UI screenshot
    └── search_results_screenshot.png # Results populated screenshot
```

---

## Build & Installation Guide

### Prerequisites

- A C++17 compatible compiler:
  - **GCC** 9+ (e.g. GCC 14/16 on MinGW-w64 or Linux)
  - **Clang** 10+
  - **MSVC** 2019 or later
- **CMake** 3.16 or later
- **Ninja** or **Make**
- **Qt 6** (Core, Gui, Widgets)

---

### Windows (MinGW-w64 / MSYS2)

1. Open PowerShell or Command Prompt with MinGW64 in your PATH:
   ```powershell
   $env:PATH = "C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;" + $env:PATH
   ```

2. Configure with CMake:
   ```powershell
   cmake -B build -G "Ninja" `
     -DCMAKE_CXX_COMPILER="C:/msys64/mingw64/bin/g++.exe" `
     -DCMAKE_MAKE_PROGRAM="C:/msys64/mingw64/bin/ninja.exe" `
     -DCMAKE_PREFIX_PATH="C:/msys64/mingw64"
   ```

3. Build the project:
   ```powershell
   cmake --build build
   ```

4. Run the application:
   ```powershell
   .\build\ParallelFileSearch.exe
   ```

---

### Windows (Visual Studio / MSVC)

1. Open **x64 Native Tools Command Prompt for VS 2022**.
2. Configure and build:
   ```cmd
   cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2019_64"
   cmake --build build --config Release
   build\Release\ParallelFileSearch.exe
   ```

---

### Linux (Ubuntu / Debian)

1. Install dependencies:
   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential cmake ninja-build qt6-base-dev
   ```

2. Configure and build:
   ```bash
   cmake -B build -G Ninja
   cmake --build build
   ```

3. Launch:
   ```bash
   ./build/ParallelFileSearch
   ```

---

### macOS

1. Install Qt 6 and CMake using Homebrew:
   ```bash
   brew install qt cmake ninja
   ```

2. Configure and build:
   ```bash
   cmake -B build -G Ninja -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
   cmake --build build
   ./build/ParallelFileSearch.app/Contents/MacOS/ParallelFileSearch
   ```

---

## Running the Automated Test Suite

All unit tests are registered with CMake's **CTest** framework.

Run the test suite from the project root:
```bash
ctest --test-dir build --output-on-failure
```

### Test Coverage Summary:
- **`ThreadPoolTest`**: Verifies task enqueueing, future resolution, high-concurrency execution (500+ tasks), active thread accounting, and pool resizing.
- **`SearchEngineTest`**: Validates filename matching, content matching, case sensitivity, recursive traversal vs non-recursive, ECMAScript regular expressions, invalid directory rejection, empty pattern handling, empty files, and binary file exclusion.
- **`CancellationTest`**: Ensures responsive cancellation while searches are active and verifies clean worker thread termination without deadlocks.
- **`ExportTest`**: Verifies RFC 4180 CSV escaping (handling embedded quotes, commas, newlines) and confirms UTF-8 Byte Order Mark (BOM) creation for Microsoft Excel.

Sample test output:
```
Test project .../build
    Start 1: ThreadPoolTest
1/4 Test #1: ThreadPoolTest ...................   Passed    0.02 sec
    Start 2: SearchEngineTest
2/4 Test #2: SearchEngineTest .................   Passed    0.18 sec
    Start 3: CancellationTest
3/4 Test #3: CancellationTest .................   Passed    0.27 sec
    Start 4: ExportTest
4/4 Test #4: ExportTest .......................   Passed    0.02 sec

100% tests passed out of 4
```

---

## Known Limitations & Future Improvements

1. **Inverted Indexing:** The current prototype performs genuine live directory scanning. A future enhancement could maintain a background file-content index for sub-millisecond repeat queries across millions of documents.
2. **Document Parsing Plugins:** Future extensions can integrate text extraction for proprietary binary document formats such as PDF, DOCX, and XLSX.
3. **GUI File Size Limit Selector:** The current 50 MB safety limit is configurable in code (`SearchOptions::maxFileSize`); adding an interactive slider in the UI would offer users even more fine-grained control.
4. **Syntax-Highlighted Match Viewer:** Adding an in-app code preview widget with line-by-line syntax highlighting for matched search results.
