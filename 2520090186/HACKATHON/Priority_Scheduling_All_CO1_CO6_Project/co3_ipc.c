#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "project.h"

#define FIFO_PATH "/tmp/priority_project_fifo"
static volatile sig_atomic_t signal_received = 0;

static void sigusr1_handler(int sig) {
    (void)sig;
    signal_received = 1;
}

static void pipe_demo(void) {
    int fd[2];
    pid_t pid;
    char msg[] = "Scheduling result sent through anonymous pipe";
    char buffer[128] = {0};

    if (pipe(fd) == -1) {
        perror("pipe");
        return;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer) - 1);
        printf("Child received: %s\n", buffer);
        close(fd[0]);
        _exit(0);
    }

    close(fd[0]);
    write(fd[1], msg, strlen(msg) + 1);
    close(fd[1]);
    waitpid(pid, NULL, 0);
}

static void fifo_demo(void) {
    const char *msg = "Priority Scheduling result via named FIFO";
    char buffer[128] = {0};
    pid_t pid;

    unlink(FIFO_PATH);
    if (mkfifo(FIFO_PATH, 0600) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        unlink(FIFO_PATH);
        return;
    }

    if (pid == 0) {
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd < 0) _exit(1);
        read(fd, buffer, sizeof(buffer) - 1);
        printf("FIFO reader received: %s\n", buffer);
        close(fd);
        _exit(0);
    }

    sleep(1);
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd >= 0) {
        write(fd, msg, strlen(msg) + 1);
        close(fd);
    } else {
        perror("open FIFO");
    }
    waitpid(pid, NULL, 0);
    unlink(FIFO_PATH);
}

static void shared_memory_demo(void) {
    int shmid = shmget(IPC_PRIVATE, 256, IPC_CREAT | 0600);
    if (shmid == -1) {
        perror("shmget");
        return;
    }

    char *shared = (char *)shmat(shmid, NULL, 0);
    if (shared == (char *)-1) {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return;
    }

    strcpy(shared, "Shared scheduling status: all processes processed");
    printf("Shared memory contains: %s\n", shared);

    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
}

static void signal_demo(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return;
    }

    printf("Sending SIGUSR1 to this process...\n");
    raise(SIGUSR1);
    printf("Signal received: %s\n",
           signal_received ? "yes" : "no");
}

void co3_demo(void) {
    int choice;

    printf("\n--- CO3: INTER-PROCESS COMMUNICATION ---\n");
    printf("1. Anonymous Pipe\n");
    printf("2. Named Pipe (FIFO)\n");
    printf("3. Shared Memory\n");
    printf("4. Signal\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: pipe_demo(); break;
        case 2: fifo_demo(); break;
        case 3: shared_memory_demo(); break;
        case 4: signal_demo(); break;
        default: printf("Invalid choice.\n");
    }
}
