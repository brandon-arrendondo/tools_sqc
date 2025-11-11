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
#include <string.h>

#define NUM_WORKERS 3
#define WORK_QUEUE_SIZE 10

typedef struct {
    int work_items[WORK_QUEUE_SIZE];
    int front, rear, count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    int shutdown;
} work_queue_t;

work_queue_t work_queue = {
    .front = 0, .rear = 0, .count = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .not_empty = PTHREAD_COND_INITIALIZER,
    .not_full = PTHREAD_COND_INITIALIZER,
    .shutdown = 0
};

// Signal handler only for critical system shutdown
void shutdown_handler(int sig) {
    if (sig == SIGTERM) {
        printf("CRITICAL: System shutdown signal - initiating worker shutdown\n");
        pthread_mutex_lock(&work_queue.mutex);
        work_queue.shutdown = 1;
        pthread_cond_broadcast(&work_queue.not_empty);
        pthread_cond_broadcast(&work_queue.not_full);
        pthread_mutex_unlock(&work_queue.mutex);
    }
}

void enqueue_work(int work_item) {
    pthread_mutex_lock(&work_queue.mutex);

    while (work_queue.count == WORK_QUEUE_SIZE && !work_queue.shutdown) {
        pthread_cond_wait(&work_queue.not_full, &work_queue.mutex);
    }

    if (!work_queue.shutdown) {
        work_queue.work_items[work_queue.rear] = work_item;
        work_queue.rear = (work_queue.rear + 1) % WORK_QUEUE_SIZE;
        work_queue.count++;
        printf("Producer: Enqueued work item %d (queue size: %d)\n", work_item, work_queue.count);
        pthread_cond_signal(&work_queue.not_empty);
    }

    pthread_mutex_unlock(&work_queue.mutex);
}

int dequeue_work() {
    int work_item = -1;

    pthread_mutex_lock(&work_queue.mutex);

    while (work_queue.count == 0 && !work_queue.shutdown) {
        pthread_cond_wait(&work_queue.not_empty, &work_queue.mutex);
    }

    if (!work_queue.shutdown && work_queue.count > 0) {
        work_item = work_queue.work_items[work_queue.front];
        work_queue.front = (work_queue.front + 1) % WORK_QUEUE_SIZE;
        work_queue.count--;
        pthread_cond_signal(&work_queue.not_full);
    }

    pthread_mutex_unlock(&work_queue.mutex);
    return work_item;
}

void* worker_thread(void* arg) {
    int worker_id = *(int*)arg;
    printf("Worker %d: Started\n", worker_id);

    while (1) {
        int work_item = dequeue_work();

        if (work_item == -1) {
            printf("Worker %d: Shutdown signal received, exiting\n", worker_id);
            break;
        }

        printf("Worker %d: Processing work item %d\n", worker_id, work_item);

        // Simulate work processing
        sleep(1);
        printf("Worker %d: Completed work item %d\n", worker_id, work_item);
    }

    return NULL;
}

void* producer_thread(void* arg) {
    printf("Producer: Starting work generation\n");

    for (int i = 1; i <= 15; i++) {
        enqueue_work(i);
        usleep(500000);  // 500ms delay between work items
    }

    printf("Producer: Finished generating work\n");
    return NULL;
}

int main() {
    printf("Using proper threading mechanisms for parallel processing, signals only for critical events (GOOD)\n");

    // Set up signal handler only for critical system events
    signal(SIGTERM, shutdown_handler);

    pthread_t workers[NUM_WORKERS];
    pthread_t producer;
    int worker_ids[NUM_WORKERS];

    // Create producer thread
    if (pthread_create(&producer, NULL, producer_thread, NULL) != 0) {
        perror("pthread_create producer");
        exit(EXIT_FAILURE);
    }

    // Create worker threads
    for (int i = 0; i < NUM_WORKERS; i++) {
        worker_ids[i] = i + 1;
        if (pthread_create(&workers[i], NULL, worker_thread, &worker_ids[i]) != 0) {
            perror("pthread_create worker");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for producer to finish
    pthread_join(producer, NULL);
    printf("Main: Producer finished, waiting for workers to complete\n");

    // Wait for all work to be processed
    pthread_mutex_lock(&work_queue.mutex);
    while (work_queue.count > 0) {
        pthread_mutex_unlock(&work_queue.mutex);
        usleep(100000);  // Check every 100ms
        pthread_mutex_lock(&work_queue.mutex);
    }

    // Signal shutdown to workers
    work_queue.shutdown = 1;
    pthread_cond_broadcast(&work_queue.not_empty);
    pthread_mutex_unlock(&work_queue.mutex);

    // Wait for all workers to finish
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
        printf("Main: Worker %d joined\n", i + 1);
    }

    printf("Parallel processing completed using proper threading mechanisms\n");

    return 0;
}