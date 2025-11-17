/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int worker1_done;
    int worker2_done;
    int proceed;
    int emergency_shutdown;
} sync_data_t;

sync_data_t sync_data = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
    0, 0, 0, 0
};

// Signal handler only for emergency shutdown (proper use of signals)
void emergency_handler(int sig) {
    if (sig == SIGTERM) {
        printf("EMERGENCY: Received termination signal - initiating emergency shutdown\n");
        sync_data.emergency_shutdown = 1;
    }
}

void* worker1_func(void* arg) {
    printf("Worker 1: Starting normal work\n");
    sleep(2);

    pthread_mutex_lock(&sync_data.mutex);
    sync_data.worker1_done = 1;
    printf("Worker 1: Work complete, signaling coordinator\n");
    pthread_cond_signal(&sync_data.condition);
    pthread_mutex_unlock(&sync_data.mutex);

    // Wait for proceed signal using condition variable
    pthread_mutex_lock(&sync_data.mutex);
    while (!sync_data.proceed && !sync_data.emergency_shutdown) {
        pthread_cond_wait(&sync_data.condition, &sync_data.mutex);
    }
    pthread_mutex_unlock(&sync_data.mutex);

    if (sync_data.emergency_shutdown) {
        printf("Worker 1: Emergency shutdown detected, exiting\n");
    } else {
        printf("Worker 1: Continuing with phase 2\n");
        sleep(1);
        printf("Worker 1: Phase 2 complete\n");
    }

    return NULL;
}

void* worker2_func(void* arg) {
    printf("Worker 2: Starting normal work\n");
    sleep(3);

    pthread_mutex_lock(&sync_data.mutex);
    sync_data.worker2_done = 1;
    printf("Worker 2: Work complete, signaling coordinator\n");
    pthread_cond_signal(&sync_data.condition);
    pthread_mutex_unlock(&sync_data.mutex);

    // Wait for proceed signal using condition variable
    pthread_mutex_lock(&sync_data.mutex);
    while (!sync_data.proceed && !sync_data.emergency_shutdown) {
        pthread_cond_wait(&sync_data.condition, &sync_data.mutex);
    }
    pthread_mutex_unlock(&sync_data.mutex);

    if (sync_data.emergency_shutdown) {
        printf("Worker 2: Emergency shutdown detected, exiting\n");
    } else {
        printf("Worker 2: Continuing with phase 2\n");
        sleep(2);
        printf("Worker 2: Phase 2 complete\n");
    }

    return NULL;
}

int main() {
    printf("Using condition variables for normal synchronization, signals only for emergencies (GOOD)\n");

    // Set up signal handler only for emergency conditions
    signal(SIGTERM, emergency_handler);

    pthread_t worker1, worker2;

    // Create worker threads
    if (pthread_create(&worker1, NULL, worker1_func, NULL) != 0) {
        perror("pthread_create worker1");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&worker2, NULL, worker2_func, NULL) != 0) {
        perror("pthread_create worker2");
        exit(EXIT_FAILURE);
    }

    // Coordinator waits for both workers using condition variables
    printf("Coordinator: Waiting for workers to complete phase 1\n");

    pthread_mutex_lock(&sync_data.mutex);
    while ((!sync_data.worker1_done || !sync_data.worker2_done) && !sync_data.emergency_shutdown) {
        pthread_cond_wait(&sync_data.condition, &sync_data.mutex);
    }

    if (sync_data.emergency_shutdown) {
        printf("Coordinator: Emergency shutdown in progress\n");
    } else {
        printf("Coordinator: Both workers done, sending proceed signal\n");
        sync_data.proceed = 1;
        pthread_cond_broadcast(&sync_data.condition);
    }
    pthread_mutex_unlock(&sync_data.mutex);

    // Wait for workers to complete
    pthread_join(worker1, NULL);
    pthread_join(worker2, NULL);

    printf("All work synchronized and complete using proper mechanisms\n");

    return 0;
}