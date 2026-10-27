# CO-1 to CO-6 Mapping for Project Review

| CO | Handout area | Project feature | Main files |
|---|---|---|---|
| CO1 | OS service layer, user/kernel, system calls, shell concepts | Linux system information + process IDs | co1_system.c |
| CO2 | Processes, process control, scheduling, priority | Priority Scheduler + fork/waitpid | co2_scheduling.c |
| CO3 | IPC | Pipe, FIFO, shared memory, signal | co3_ipc.c |
| CO4 | Virtual memory and process memory | Stack/heap + dynamic allocation | co4_memory.c |
| CO5 | File systems and Linux file I/O | open/read/write/close + stat/inode | co5_files.c |
| CO6 | Concurrency and synchronization | pthreads, mutex, condition variable, semaphore, deadlock/starvation concepts | co6_threads.c |

## What to say

**CO1:** "We demonstrate Linux OS services and system calls from a user-space C program."

**CO2:** "Our main module implements non-preemptive Priority Scheduling and also demonstrates process creation and synchronization using fork and waitpid."

**CO3:** "Our IPC module demonstrates communication between processes using pipes, FIFO, shared memory and signals."

**CO4:** "Our memory module demonstrates process memory concepts and dynamic memory allocation using malloc/calloc/realloc/free."

**CO5:** "Our file module demonstrates Linux file descriptors and file I/O using open, read, write, close and stat."

**CO6:** "Our concurrency module demonstrates POSIX threads, mutexes, condition variables and counting semaphores, and explains race conditions, deadlocks and starvation."

## Important distinction

The course handout's CO6 includes POSIX threads, mutexes, condition variables, counting semaphores, race conditions, deadlocks and starvation. The project demonstrates these as practical modules; it does not intentionally create a permanent deadlock because that would hang the application.
