# Priority Scheduling & Linux Systems Programming Project

## Purpose

This project keeps **Priority Scheduling** as the main feature and adds practical Linux demonstrations related to all six course outcomes (CO1-CO6) from the provided OSSP handout.

## CO Mapping

### CO1 - OS as a Service Layer
- `getpid()`, `getppid()`
- `uname()`
- Demonstrates user-space interaction with Linux system services.

### CO2 - Processes and Process Control
- Non-preemptive Priority Scheduling
- Arrival Time, Burst Time, Priority
- Completion Time (CT)
- Turnaround Time (TAT)
- Waiting Time (WT)
- Average TAT / WT
- `fork()` and `waitpid()`

### CO3 - Inter-Process Communication
- Anonymous pipe
- Named pipe (FIFO)
- Shared memory
- POSIX signal demonstration

### CO4 - Memory Management
- Process memory layout concept
- Stack and heap addresses
- `malloc()`, `calloc()`, `realloc()`, `free()`

### CO5 - File Systems and File I/O
- `open()`, `read()`, `write()`, `close()`
- `stat()` and inode metadata
- Saves a scheduling result file

### CO6 - Concurrency and Synchronization
- POSIX threads
- Mutex
- Condition variables
- Counting semaphore
- Race condition concept
- Deadlock concept
- Starvation concept

## Priority Scheduling formulas

- CT = time when the process finishes
- TAT = CT - AT
- WT = TAT - BT
- Average TAT = Sum(TAT) / Number of Processes
- Average WT = Sum(WT) / Number of Processes

## Ubuntu setup

```bash
sudo apt update
sudo apt install build-essential
cd ~/priority_scheduling_all_COs
make
./priority_system
```

If you do not want to use `make`:

```bash
gcc -Wall -Wextra -O2 main.c co1_system.c co2_scheduling.c co3_ipc.c co4_memory.c co5_files.c co6_threads.c -o priority_system -pthread
./priority_system
```

## Suggested review demonstration

1. Select **2 - Priority Scheduling** and enter 4 processes.
2. Show Gantt sequence and CT/TAT/WT table.
3. Select **3 - IPC** and demonstrate anonymous pipe.
4. Select **4 - Memory Management** and show heap/stack.
5. Select **5 - File System** and create/read the result file.
6. Select **6 - Concurrency** and demonstrate mutex, condition variable, and semaphore.
7. Explain CO1 using the system-call demo.

## Important scope note

This is a demonstration project. It is designed to visibly connect practical Linux/C concepts to the six COs. It does not claim that every CO topic is fully implemented as a production-grade subsystem. For the review, explain exactly which feature demonstrates each CO.
