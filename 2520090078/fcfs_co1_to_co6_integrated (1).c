#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

typedef struct {
    int n;
    int bt[MAX], ct[MAX], tat[MAX], wt[MAX];
    float avg_tat, avg_wt;
    long long tat_sum_thread, wt_sum_thread;
} FCFSReport;

// CO-3: Signal handler: only sets a signal-safe flag.
volatile sig_atomic_t signal_received = 0;
void handle_signal(int sig) {
    (void)sig;
    signal_received = 1;
}

// CO-6: Two threads use a mutex while updating shared totals.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long long thread_totals[2] = {0, 0};

typedef struct {
    int *values;
    int n;
    int slot;
} ThreadData;

void *sum_values(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int j = 0; j < data->n; j++) {
        pthread_mutex_lock(&mutex);
        thread_totals[data->slot] += data->values[j];
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// CO-5: Child saves the FCFS report and reads it back using file system calls.
void save_and_display_file(const FCFSReport *r) {
    char text[8192];
    int used = snprintf(text, sizeof(text),
                        "FCFS Scheduling Report\nProcess\tBT\tCT\tTAT\tWT\n");

    for (int j = 0; j < r->n && used > 0 && used < (int)sizeof(text); j++) {
        used += snprintf(text + used, sizeof(text) - (size_t)used,
                         "P%d\t%d\t%d\t%d\t%d\n", j + 1,
                         r->bt[j], r->ct[j], r->tat[j], r->wt[j]);
    }
    if (used > 0 && used < (int)sizeof(text)) {
        used += snprintf(text + used, sizeof(text) - (size_t)used,
                         "Average TAT = %.2f\nAverage WT = %.2f\n",
                         r->avg_tat, r->avg_wt);
    }
    if (used <= 0 || used >= (int)sizeof(text)) {
        printf("Could not prepare report.\n");
        return;
    }

    int fd = open("fcfs_report.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return; }
    if (write(fd, text, (size_t)used) < 0) perror("write");
    close(fd);

    fd = open("fcfs_report.txt", O_RDONLY);
    if (fd < 0) { perror("open for reading"); return; }

    printf("\nCO-5: Reading the saved report from fcfs_report.txt\n");
    fflush(stdout);
    char buffer[256];
    ssize_t count;
    while ((count = read(fd, buffer, sizeof(buffer))) > 0)
        write(STDOUT_FILENO, buffer, (size_t)count);
    close(fd);
}

int main(void) {
    int n, i;
    int bt[100], ct[100], tat[100], wt[100];
    float avg_tat = 0, avg_wt = 0;

    // CO-1: The shell starts this user-space program. write() crosses into the kernel.
    printf("Enter number of processes: ");
    scanf("%d", &n);
    if (n < 1 || n > MAX) {
        printf("Enter between 1 and %d processes.\n", MAX);
        return 1;
    }

    // Input burst time
    printf("\nEnter Burst Time for each process:\n");
    for (i = 0; i < n; i++) {
        printf("P%d: ", i + 1);
        scanf("%d", &bt[i]);
    }

    // FCFS scheduling (original calculation)
    ct[0] = bt[0];
    for (i = 1; i < n; i++) {
        ct[i] = ct[i - 1] + bt[i];
    }

    // Calculate TAT and WT (original calculation)
    for (i = 0; i < n; i++) {
        tat[i] = ct[i];       // Arrival time = 0
        wt[i] = tat[i] - bt[i];
        avg_tat += tat[i];
        avg_wt += wt[i];
    }

    // Display results (original table and averages)
    printf("\n--------------------------------------------------");
    printf("\nProcess\tBT\tCT\tTAT\tWT");
    printf("\n--------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\n", i + 1, bt[i], ct[i], tat[i], wt[i]);
    }
    avg_tat = avg_tat / n;
    avg_wt = avg_wt / n;
    printf("\nAverage Turnaround Time = %.2f", avg_tat);
    printf("\nAverage Waiting Time    = %.2f\n", avg_wt);

    // CO-1: A direct system-call example.
    fflush(stdout);
    const char msg[] = "\nCO-1: write() system call sends output to the terminal.\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    printf("Current process ID = %ld\n", (long)getpid());

    // CO-6: Threads verify the totals from the FCFS arrays.
    printf("\nCO-6: Threads calculate FCFS totals with mutex protection.\n");
    pthread_t t1, t2;
    ThreadData d1 = {tat, n, 0}, d2 = {wt, n, 1};
    int ok1 = (pthread_create(&t1, NULL, sum_values, &d1) == 0);
    int ok2 = (pthread_create(&t2, NULL, sum_values, &d2) == 0);
    if (ok1) pthread_join(t1, NULL);
    if (ok2) pthread_join(t2, NULL);
    if (ok1 && ok2)
        printf("Thread totals: TAT sum = %lld, WT sum = %lld\n", thread_totals[0], thread_totals[1]);
    else
        printf("A thread could not be created.\n");
    pthread_mutex_destroy(&mutex);

    // CO-4: Put the FCFS report in shared virtual memory before fork().
    FCFSReport *shared = mmap(NULL, sizeof(FCFSReport), PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) { perror("mmap"); return 1; }
    shared->n = n;
    memcpy(shared->bt, bt, n * sizeof(int));
    memcpy(shared->ct, ct, n * sizeof(int));
    memcpy(shared->tat, tat, n * sizeof(int));
    memcpy(shared->wt, wt, n * sizeof(int));
    shared->avg_tat = avg_tat;
    shared->avg_wt = avg_wt;
    shared->tat_sum_thread = thread_totals[0];
    shared->wt_sum_thread = thread_totals[1];
    printf("\nCO-4: FCFS results are stored in mmap() shared memory.\n");

    // CO-3: Pipe sends a ready message; SIGUSR1 notifies the child asynchronously.
    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); munmap(shared, sizeof(*shared)); return 1; }

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handle_signal;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction"); close(fd[0]); close(fd[1]); munmap(shared, sizeof(*shared)); return 1;
    }

    sigset_t block_set, old_mask;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &block_set, &old_mask) == -1) {
        perror("sigprocmask"); close(fd[0]); close(fd[1]); munmap(shared, sizeof(*shared)); return 1;
    }

    printf("\nCO-2: fork() creates a child process.\n");
    printf("Parent process group = %ld, session = %ld\n", (long)getpgrp(), (long)getsid(0));
    fflush(NULL);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork"); close(fd[0]); close(fd[1]);
        sigprocmask(SIG_SETMASK, &old_mask, NULL); munmap(shared, sizeof(*shared)); return 1;
    }

    if (pid == 0) {
        close(fd[1]);
        char ready;
        if (read(fd[0], &ready, 1) != 1) _exit(1);
        close(fd[0]);

        // sigsuspend avoids missing a signal that arrives just before the wait.
        while (!signal_received) sigsuspend(&old_mask);
        sigprocmask(SIG_SETMASK, &old_mask, NULL);

        printf("\nChild PID = %ld, Parent PID = %ld\n", (long)getpid(), (long)getppid());
        printf("Child process group = %ld, session = %ld\n", (long)getpgrp(), (long)getsid(0));
        printf("CO-3: Child got pipe message '%c' and SIGUSR1.\n", ready);
        fflush(stdout);

        save_and_display_file(shared); // CO-5: child performs file I/O on FCFS results.
        fflush(stdout);
        munmap(shared, sizeof(*shared));
        _exit(0);
    } else {
        close(fd[0]);
        char ready = 'R';
        write(fd[1], &ready, 1);
        close(fd[1]);
        kill(pid, SIGUSR1);

        int status;
        waitpid(pid, &status, 0); // CO-2: parent waits for child termination.
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
        if (WIFEXITED(status))
            printf("\nCO-2: waitpid() confirms child exit status = %d\n", WEXITSTATUS(status));
    }

    munmap(shared, sizeof(*shared));
    return 0;
}
