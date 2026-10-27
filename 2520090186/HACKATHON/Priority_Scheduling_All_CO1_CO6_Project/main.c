#include <stdio.h>
#include "project.h"

static void menu(void) {
    printf("\n===============================================\n");
    printf(" PRIORITY SCHEDULING & LINUX SYSTEMS PROJECT\n");
    printf("===============================================\n");
    printf("1. CO1 - System Calls / OS Service Layer\n");
    printf("2. CO2 - Process Management + Priority Scheduling\n");
    printf("3. CO3 - IPC (Pipe / FIFO / Shared Memory / Signal)\n");
    printf("4. CO4 - Memory Management\n");
    printf("5. CO5 - File System & File I/O\n");
    printf("6. CO6 - Threads & Synchronization\n");
    printf("7. Run All CO Demonstrations\n");
    printf("0. Exit\n");
    printf("Enter choice: ");
}

int main(void) {
    int choice;

    do {
        menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            return 1;
        }

        switch (choice) {
            case 1: co1_demo(); break;
            case 2: co2_demo(); break;
            case 3: co3_demo(); break;
            case 4: co4_demo(); break;
            case 5: co5_demo(); break;
            case 6: co6_demo(); break;
            case 7:
                co1_demo();
                co2_demo();
                co3_demo();
                co4_demo();
                co5_demo();
                co6_demo();
                break;
            case 0:
                printf("Exiting project. Thank you!\n");
                break;
            default:
                printf("Please choose 0-7.\n");
        }
    } while (choice != 0);

    return 0;
}
