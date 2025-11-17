/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_SIZE 50

/* Violation: Accessing shared queue system in signal handler */
typedef struct {
    int priority;
    char message[128];
    int timestamp;
    int sequence_id;
} queue_item_t;

typedef struct {
    queue_item_t items[QUEUE_SIZE];
    int head;
    int tail;
    int count;
    int total_enqueued;
    int total_dequeued;
    int peak_usage;
    char queue_name[64];
} priority_queue_t;

priority_queue_t global_high_priority_queue = {0};
priority_queue_t global_low_priority_queue = {0};

void init_queue(priority_queue_t *queue, const char *name) {
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->total_enqueued = 0;
    queue->total_dequeued = 0;
    queue->peak_usage = 0;
    strcpy(queue->queue_name, name);
}

int enqueue(priority_queue_t *queue, int priority, const char *message, int timestamp) {
    if (queue->count >= QUEUE_SIZE) {
        return 0;  /* Queue full */
    }

    queue_item_t *item = &queue->items[queue->tail];
    item->priority = priority;
    strcpy(item->message, message);
    item->timestamp = timestamp;
    item->sequence_id = queue->total_enqueued + 1;

    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->count++;
    queue->total_enqueued++;

    if (queue->count > queue->peak_usage) {
        queue->peak_usage = queue->count;
    }

    return 1;  /* Success */
}

int dequeue(priority_queue_t *queue, queue_item_t *item) {
    if (queue->count == 0) {
        return 0;  /* Queue empty */
    }

    *item = queue->items[queue->head];
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->count--;
    queue->total_dequeued++;

    return 1;  /* Success */
}

void unsafe_handler(int sig) {
    /* Violation: Accessing shared queue systems in signal handler */
    char signal_message[128];
    sprintf(signal_message, "Emergency signal %d received", sig);

    if (sig == SIGUSR1) {
        /* Add high priority emergency message */
        enqueue(&global_high_priority_queue, 999, signal_message, sig);
        sprintf(global_high_priority_queue.queue_name, "emergency_high_%d", sig);
    } else if (sig == SIGUSR2) {
        /* Add low priority message and process some items */
        enqueue(&global_low_priority_queue, 1, signal_message, sig);

        /* Try to process some items from high priority queue */
        queue_item_t processed_item;
        if (dequeue(&global_high_priority_queue, &processed_item)) {
            /* Modify the processed item */
            processed_item.priority = 0;  /* Mark as processed */
        }
    }

    /* Update queue statistics */
    global_high_priority_queue.peak_usage += sig % 3;
    global_low_priority_queue.peak_usage += sig % 2;

    printf("Handler: high_queue(count=%d, peak=%d), low_queue(count=%d, peak=%d)\n",
           global_high_priority_queue.count, global_high_priority_queue.peak_usage,
           global_low_priority_queue.count, global_low_priority_queue.peak_usage);
}

int main() {
    printf("Demonstrating unsafe shared queue system access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize queues */
    init_queue(&global_high_priority_queue, "high_priority_queue");
    init_queue(&global_low_priority_queue, "low_priority_queue");

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 30; i++) {
        /* Main program also uses the queues */
        char main_message[128];
        sprintf(main_message, "Main program task %d", i);

        /* Add items to queues based on conditions */
        if (i % 3 == 0) {
            enqueue(&global_high_priority_queue, 50 + i, main_message, i);
        } else {
            enqueue(&global_low_priority_queue, 10 + i, main_message, i);
        }

        /* Process items from queues */
        queue_item_t processed_item;
        if (i % 4 == 3) {
            if (dequeue(&global_high_priority_queue, &processed_item)) {
                printf("Main: Processed high priority: %s (priority=%d, seq=%d)\n",
                       processed_item.message, processed_item.priority, processed_item.sequence_id);
            }
        }

        if (i % 5 == 4) {
            if (dequeue(&global_low_priority_queue, &processed_item)) {
                printf("Main: Processed low priority: %s (priority=%d, seq=%d)\n",
                       processed_item.message, processed_item.priority, processed_item.sequence_id);
            }
        }

        /* Update queue names */
        if (i % 7 == 6) {
            sprintf(global_high_priority_queue.queue_name, "main_high_%d", i);
            sprintf(global_low_priority_queue.queue_name, "main_low_%d", i);
        }

        printf("Main[%d]: high(cnt=%d,enq=%d,deq=%d) low(cnt=%d,enq=%d,deq=%d)\n",
               i,
               global_high_priority_queue.count,
               global_high_priority_queue.total_enqueued,
               global_high_priority_queue.total_dequeued,
               global_low_priority_queue.count,
               global_low_priority_queue.total_enqueued,
               global_low_priority_queue.total_dequeued);

        usleep(120000);
    }

    printf("Final queue states:\n");
    printf("High priority: count=%d, total_enqueued=%d, peak=%d\n",
           global_high_priority_queue.count,
           global_high_priority_queue.total_enqueued,
           global_high_priority_queue.peak_usage);
    printf("Low priority: count=%d, total_enqueued=%d, peak=%d\n",
           global_low_priority_queue.count,
           global_low_priority_queue.total_enqueued,
           global_low_priority_queue.peak_usage);

    return 0;
}