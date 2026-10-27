#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "project.h"

#define RESULT_FILE "scheduling_results.txt"

static void file_demo(void) {
    int fd;
    const char *text =
        "Priority Scheduling Result\n"
        "P1 AT=2 BT=5 Priority=2 CT=7 TAT=5 WT=0\n"
        "P2 AT=3 BT=6 Priority=1 CT=13 TAT=10 WT=4\n";

    printf("\n--- CO5: FILE SYSTEM & FILE I/O ---\n");

    fd = open(RESULT_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }

    if (write(fd, text, strlen(text)) == -1)
        perror("write");

    close(fd);

    fd = open(RESULT_FILE, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    char buffer[512] = {0};
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes >= 0) {
        buffer[bytes] = '\0';
        printf("Contents read using read():\n%s", buffer);
    } else {
        perror("read");
    }

    close(fd);

    printf("Result file created: %s\n", RESULT_FILE);
    printf("Demonstrated: open(), read(), write(), close() and file descriptor usage.\n");
}

static void file_info_demo(void) {
    struct stat st;
    if (stat(RESULT_FILE, &st) == -1) {
        printf("Run File I/O first to create %s.\n", RESULT_FILE);
        return;
    }

    printf("File: %s\n", RESULT_FILE);
    printf("Size: %lld bytes\n", (long long)st.st_size);
    printf("Inode: %lld\n", (long long)st.st_ino);
    printf("File mode: %o\n", (unsigned)(st.st_mode & 0777));
}

void co5_demo(void) {
    int choice;

    printf("\n--- CO5: FILE SYSTEM ---\n");
    printf("1. File I/O demonstration\n");
    printf("2. File metadata / inode demonstration\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1)
        file_demo();
    else if (choice == 2)
        file_info_demo();
    else
        printf("Invalid choice.\n");
}
