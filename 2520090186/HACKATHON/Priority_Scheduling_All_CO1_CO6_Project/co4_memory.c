#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "project.h"

static void memory_layout_demo(void) {
    int stack_var = 10;
    int *heap_var = malloc(sizeof(int));

    if (!heap_var) {
        perror("malloc");
        return;
    }

    *heap_var = 20;

    printf("\n--- CO4: MEMORY MANAGEMENT ---\n");
    printf("Stack variable address : %p\n", (void *)&stack_var);
    printf("Heap variable address  : %p\n", (void *)heap_var);
    printf("Text/Data/Heap/Stack are parts of a process address space.\n");

    free(heap_var);
}

static void dynamic_memory_demo(void) {
    int n;
    printf("Enter number of scheduling records to allocate: ");
    scanf("%d", &n);

    if (n <= 0 || n > 1000) {
        printf("Use a value between 1 and 1000.\n");
        return;
    }

    int *data = calloc((size_t)n, sizeof(int));
    if (!data) {
        perror("calloc");
        return;
    }

    for (int i = 0; i < n; i++)
        data[i] = (i + 1) * 10;

    printf("Allocated %d integers using calloc().\n", n);
    printf("First value: %d\n", data[0]);

    int new_n = n + 2;
    int *temp = realloc(data, (size_t)new_n * sizeof(int));
    if (!temp) {
        free(data);
        perror("realloc");
        return;
    }

    data = temp;
    data[n] = 999;
    data[n + 1] = 1000;

    printf("Resized memory using realloc(); new size = %d integers.\n", new_n);
    printf("New values: %d, %d\n", data[n], data[n + 1]);

    free(data);
    printf("Memory released using free().\n");
}

void co4_demo(void) {
    int choice;

    printf("\n--- CO4: MEMORY MANAGEMENT ---\n");
    printf("1. Process memory layout\n");
    printf("2. Dynamic memory allocation\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1)
        memory_layout_demo();
    else if (choice == 2)
        dynamic_memory_demo();
    else
        printf("Invalid choice.\n");
}
