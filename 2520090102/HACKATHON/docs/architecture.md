# Architecture & Design Specification: Parallel File Search

## 1. System Overview

**Parallel File Search Using Threads** is a high-performance desktop application engineered in modern C++ (C++17) and Qt 6. It performs concurrent directory discovery and file-content pattern matching using standard library threading primitives (`std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`).

The architecture strictly decouples the core search engine logic (`pfs_core`) from the graphical user interface (`ParallelFileSearch`), enabling independent headless testing, modular maintenance, and zero GUI lockups during intensive file system I/O.

---

## 2. Component Hierarchy & Responsibilities

The codebase is organized into eight specialized components:

```
+-------------------------------------------------------------------------+
|                               MainWindow                                |
|  - Modern Dark Theme (Qt Widgets: QTableWidget, QProgressBar, etc.)     |
|  - User inputs, sorting, filtering, context menu, double-click launch   |
+------------------------------------+------------------------------------+
                                     | Qt Signals / Slots (Queued)
                                     v
+-------------------------------------------------------------------------+
|                            SearchController                             |
|  - Qt QObject mediator running on the main UI thread                    |
|  - Marshals SearchStats & SearchResult metatypes across thread boundary  |
|  - Controls engine lifecycle (start, stop, clear, export)               |
+------------------------------------+------------------------------------+
                                     | Direct C++ API Call
                                     v
+-------------------------------------------------------------------------+
|                              SearchEngine                               |
|  - Runs asynchronous coordinator thread (`m_coordinatorThread`)         |
|  - Discovers files via `std::filesystem::recursive_directory_iterator`   |
|  - Batches files (25 files/task) to minimize queue lock contention      |
|  - Aggregates atomic statistics (`m_filesScanned`, `m_matchesFound`)     |
+-------------------+--------------------------------+--------------------+
                    | Submits Batches                | Logs Milestones
                    v                                v
+----------------------------------+   +----------------------------------+
|            ThreadPool            |   |              Logger              |
| - Fixed worker pool (`std::thread`)| | - Thread-safe ring/entry list    |
| - `std::mutex` & condition vars  |   | - Leveled logging (INFO, WARN,   |
| - Atomic active worker counter   |   |   ERROR, SUCCESS)                |
| - Graceful cancellation & waitAll|   | - Real-time callback & export    |
+-------------------+--------------+   +----------------------------------+
                    | Executes
                    v
+-------------------------------------------------------------------------+
|                               SearchWorker                              |
|  - Content pattern matching (case-sensitive, insensitive, std::regex)   |
|  - Filename matching                                                    |
|  - Binary file detection (checks null bytes & non-printable bytes)      |
|  - Incremental line-by-line reading via `std::ifstream`                 |
|  - File size capping (skips files > 50 MB to prevent memory bloat)      |
+------------------------------------+------------------------------------+
                                     | Produces Matches
                                     v
+-------------------------------------------------------------------------+
|                     SearchResult & ExportManager                        |
|  - Structured match records (ID, Filename, Path, Line, Text, Status)    |
|  - RFC 4180 CSV export with field escaping & UTF-8 BOM for MS Excel     |
+-------------------------------------------------------------------------+
```

---

## 3. Concurrency & Synchronization Model

### Threading Architecture
1. **Main GUI Thread:**
   - Runs the Qt event loop (`QApplication::exec()`).
   - Renders widget updates, manages table sorting/filtering, and handles user interactions.
   - Never performs blocking file operations.

2. **Coordinator Thread:**
   - Spawned by `SearchEngine::startAsync()`.
   - Traverses the directory hierarchy using `std::filesystem`.
   - Groups discovered files into chunks (default 25 files per batch) and enqueues tasks into the `ThreadPool`.
   - Waits for thread pool drain via `ThreadPool::waitAll()`.
   - Emits completion notifications.

3. **Worker Thread Pool:**
   - Configurable number of worker threads (defaults to `std::thread::hardware_concurrency()`).
   - Pulls batches from a synchronized task queue (`std::queue<std::function<void()>>`).
   - Employs condition variables (`m_cvTaskAvailable` and `m_cvTasksFinished`) to eliminate busy waiting.
   - Tracks active worker count via `std::atomic<int> m_activeWorkers`.

### Atomic State & Memory Ordering
- `m_cancelRequested` (`std::atomic<bool>`): Read with `std::memory_order_relaxed` inside search loops; written with `std::memory_order_release`.
- `m_isRunning` (`std::atomic<bool>`): Coordinates engine busy state.
- `m_filesDiscovered`, `m_filesScanned`, `m_matchesFound`: Incremented atomically via `fetch_add(1, std::memory_order_relaxed)` by worker threads without requiring coarse mutex locks.
- `m_resultsMutex` (`std::mutex`): Guards the accumulated `std::vector<SearchResult>` collection.

### Responsive Cancellation Strategy
Cancellation is instantaneous and safe:
1. User clicks **Stop Search** in the GUI.
2. `SearchController::cancelSearch()` calls `SearchEngine::cancel()`.
3. `m_cancelRequested` is atomically set to `true`.
4. `ThreadPool::cancel()` acquires the queue lock, flushes all unstarted tasks, and wakes up all sleeping workers.
5. In-flight file reads periodically test `m_cancelRequested.load(std::memory_order_relaxed)` on each line and break immediately.
6. The coordinator thread exits and joins cleanly without deadlocks or thread leaks.

### UI Event Loop Throttling
In high-performance directory searches scanning tens of thousands of files per second, firing individual Qt events per file would overwhelm the Qt event loop. `SearchEngine` employs a dual-throttling mechanism:
- Worker tasks report matches in batches.
- Progress updates are throttled to a minimum interval of 40ms (~25 FPS), ensuring butter-smooth UI rendering while accurately reflecting real-time numbers.
- A guaranteed final notification is dispatched upon completion.

---

## 4. File I/O & Safety Policies

- **Binary Detection:** Before scanning file contents, `SearchWorker::isBinaryFile()` reads the first 2048 bytes. If null bytes (`\0`) or >15% non-printable ASCII control characters are detected, content scanning is skipped, preventing garbage text output.
- **Incremental Line Reading:** Files are read line-by-line via `std::ifstream` and `std::getline()`. Memory is bounded to the size of a single line, rather than reading entire gigabyte files into RAM.
- **Filesize Cap:** Files exceeding 50 MB are skipped with a warning logged to the activity log.
- **Symlink & Permission Safety:** Traversals use `std::filesystem::directory_options::skip_permission_denied` and check `is_regular_file()`, preventing crashes on protected system directories or recursive symlink loops.
- **Unicode Support:** File paths and content strings are processed with full UTF-8 encoding support.
