#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "project.h"

/*
 * CO6: Concurrency and Synchronization.
 * Demonstrates:
 *  - POSIX threads
 *  - mutex-protected shared data
 *  - condition variable producer/consumer
 *  - counting semaphore
 *  - safe discussion of deadlock/starvation
 */

static int shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *counter_worker(void *arg) {
    int loops = *(int *)arg;
    for (int i = 0; i < loops; i++) {
        pthread_mutex_lock(&counter_mutex);
        shared_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

static void pthread_mutex_demo(void) {
    pthread_t t1, t2;
    int loops = 100000;

    shared_counter = 0;

    pthread_create(&t1, NULL, counter_worker, &loops);
    pthread_create(&t2, NULL, counter_worker, &loops);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Two POSIX threads updated one shared counter.\n");
    printf("Mutex-protected final counter = %d (expected %d)\n",
           shared_counter, loops * 2);
}

#define BUFFER_SIZE 5
static int buffer[BUFFER_SIZE];
static int in_pos = 0, out_pos = 0, count = 0;
static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

static void *producer(void *arg) {
    (void)arg;

    for (int item = 1; item <= 5; item++) {
        pthread_mutex_lock(&buffer_mutex);

        while (count == BUFFER_SIZE)
            pthread_cond_wait(&not_full, &buffer_mutex);

        buffer[in_pos] = item;
        in_pos = (in_pos + 1) % BUFFER_SIZE;
        count++;

        printf("Producer produced scheduling record %d\n", item);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&buffer_mutex);
    }
    return NULL;
}

static void *consumer(void *arg) {
    (void)arg;

    for (int i = 1; i <= 5; i++) {
        pthread_mutex_lock(&buffer_mutex);

        while (count == 0)
            pthread_cond_wait(&not_empty, &buffer_mutex);

        int item = buffer[out_pos];
        out_pos = (out_pos + 1) % BUFFER_SIZE;
        count--;

        printf("Consumer processed scheduling record %d\n", item);

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&buffer_mutex);
    }
    return NULL;
}

static void condition_demo(void) {
    pthread_t prod, cons;

    in_pos = out_pos = count = 0;

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    printf("Condition-variable producer/consumer completed safely.\n");
}

static void semaphore_worker(sem_t *sem, int id) {
    sem_wait(sem);
    printf("Thread %d entered limited resource.\n", id);
    usleep(100000);
    printf("Thread %d leaving limited resource.\n", id);
    sem_post(sem);
}

static void *sem_thread(void *arg) {
    int id = *(int *)arg;
    extern sem_t resource_sem;
    semaphore_worker(&resource_sem, id);
    return NULL;
}

sem_t resource_sem;

static void semaphore_demo(void) {
    pthread_t threads[4];
    int ids[4] = {1, 2, 3, 4};

    sem_init(&resource_sem, 0, 2);

    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, sem_thread, &ids[i]);

    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    sem_destroy(&resource_sem);
    printf("Counting semaphore allowed at most 2 threads at once.\n");
}

static void concurrency_notes(void) {
    printf("\n--- CO6 CONCURRENCY CONCEPTS ---\n");
    printf("Race condition: multiple threads access shared data without proper synchronization.\n");
    printf("Deadlock: processes/threads wait forever for resources held by each other.\n");
    printf("Starvation: a process/thread waits indefinitely because others keep receiving resources.\n");
    printf("This project prevents deadlock in its demos by using a consistent lock order\n");
    printf("and finite demonstrations rather than intentionally hanging the program.\n");
}

void co6_demo(void) {
    int choice;

    printf("\n--- CO6: THREADS & SYNCHRONIZATION ---\n");
    printf("1. POSIX threads + mutex\n");
    printf("2. Condition variable producer/consumer\n");
    printf("3. Counting semaphore\n");
    printf("4. Race condition / deadlock / starvation concepts\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: pthread_mutex_demo(); break;
        case 2: condition_demo(); break;
        case 3: semaphore_demo(); break;
        case 4: concurrency_notes(); break;
        default: printf("Invalid choice.\n");
    }
}
