#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "project.h"

/*
 * CO1: OS as a Service Layer
 * Demonstrates user-space -> system-call interface using Linux APIs.
 */
void co1_demo(void) {
    struct utsname info;

    printf("\n--- CO1: OS AS A SERVICE LAYER ---\n");
    printf("Current Process ID (getpid): %d\n", (int)getpid());
    printf("Parent Process ID (getppid): %d\n", (int)getppid());

    if (uname(&info) == 0) {
        printf("Operating System: %s\n", info.sysname);
        printf("Kernel Release: %s\n", info.release);
        printf("Machine: %s\n", info.machine);
    } else {
        perror("uname");
    }

    printf("Concept: a user-space program requests OS services through system calls.\n");
}
