#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "project.h"

#define MAX_P 50

typedef struct {
    int pid;
    int at;
    int bt;
    int priority;
    int ct;
    int tat;
    int wt;
    int completed;
} Process;

/*
 * CO2: Process management + user-level priority scheduling.
 * Non-preemptive priority scheduling.
 * Assumption: smaller priority number = higher priority.
 * Equal priority: FCFS by arrival time, then process number.
 */
static int choose_process(Process p[], int n, int time) {
    int best = -1;
    for (int i = 0; i < n; i++) {
        if (!p[i].completed && p[i].at <= time) {
            if (best == -1 ||
                p[i].priority < p[best].priority ||
                (p[i].priority == p[best].priority && p[i].at < p[best].at) ||
                (p[i].priority == p[best].priority &&
                 p[i].at == p[best].at && p[i].pid < p[best].pid)) {
                best = i;
            }
        }
    }
    return best;
}

static void run_scheduling(void) {
    Process p[MAX_P];
    int n, time = 0, done = 0;
    double total_tat = 0.0, total_wt = 0.0;

    printf("\n--- CO2: PRIORITY SCHEDULING ---\n");
    printf("Smaller priority number = higher priority.\n");
    printf("Enter number of processes (1-%d): ", MAX_P);
    scanf("%d", &n);

    if (n < 1 || n > MAX_P) {
        printf("Invalid number of processes.\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        p[i].completed = 0;
        printf("\nP%d Arrival Time: ", i + 1);
        scanf("%d", &p[i].at);
        printf("P%d Burst Time: ", i + 1);
        scanf("%d", &p[i].bt);
        printf("P%d Priority: ", i + 1);
        scanf("%d", &p[i].priority);
        if (p[i].at < 0 || p[i].bt <= 0 || p[i].priority < 0) {
            printf("Invalid process data.\n");
            return;
        }
    }

    printf("\nGantt sequence: ");
    while (done < n) {
        int selected = choose_process(p, n, time);

        if (selected == -1) {
            time++;
            continue;
        }

        printf("[ P%d %d-%d ] ", p[selected].pid, time,
               time + p[selected].bt);

        time += p[selected].bt;
        p[selected].ct = time;
        p[selected].tat = p[selected].ct - p[selected].at;
        p[selected].wt = p[selected].tat - p[selected].bt;
        p[selected].completed = 1;
        total_tat += p[selected].tat;
        total_wt += p[selected].wt;
        done++;
    }

    printf("\n\nProcess\tAT\tBT\tPriority\tCT\tTAT\tWT\n");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt, p[i].priority,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage Turnaround Time = %.2f\n", total_tat / n);
    printf("Average Waiting Time    = %.2f\n", total_wt / n);
}

static void process_demo(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        printf("\nChild process: PID=%d, Parent PID=%d\n",
               (int)getpid(), (int)getppid());
        _exit(0);
    } else {
        int status;
        printf("Parent process: PID=%d created child PID=%d\n",
               (int)getpid(), (int)pid);
        waitpid(pid, &status, 0);
        printf("Parent: child has terminated; waitpid() completed.\n");
    }
}

void co2_demo(void) {
    int choice;
    printf("\n--- CO2 MENU ---\n");
    printf("1. Priority Scheduling\n");
    printf("2. fork()/waitpid() Process Demonstration\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1)
        run_scheduling();
    else if (choice == 2)
        process_demo();
    else
        printf("Invalid choice.\n");
}
