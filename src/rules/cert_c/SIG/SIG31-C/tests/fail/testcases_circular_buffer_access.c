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

#define BUFFER_SIZE 64

typedef struct {
    char data[BUFFER_SIZE][128];
    int head;
    int tail;
    int count;
    int overruns;
    char last_operation[32];
} circular_buffer_t;

typedef struct {
    int queue_data[BUFFER_SIZE];
    int front;
    int rear;
    int size;
    int max_size_reached;
    double average_size;
} queue_t;

circular_buffer_t global_log_buffer = {0};
queue_t global_event_queue = {0};

void enqueue_event(queue_t *q, int value) {
    if (q->size < BUFFER_SIZE) {
        q->queue_data[q->rear] = value;
        q->rear = (q->rear + 1) % BUFFER_SIZE;
        q->size++;
        if (q->size > q->max_size_reached) {
            q->max_size_reached = q->size;
        }
    }
}

void add_log_entry(circular_buffer_t *buf, const char *entry) {
    strcpy(buf->data[buf->head], entry);
    buf->head = (buf->head + 1) % BUFFER_SIZE;
    if (buf->count < BUFFER_SIZE) {
        buf->count++;
    } else {
        buf->tail = (buf->tail + 1) % BUFFER_SIZE;
        buf->overruns++;
    }
}

void unsafe_handler(int sig) {
    /* Violation: Accessing shared circular buffers and queues in signal handler */

    char log_entry[128];
    sprintf(log_entry, "Signal %d received in handler", sig);
    add_log_entry(&global_log_buffer, log_entry);
    strcpy(global_log_buffer.last_operation, "signal_add");

    /* Add signal to event queue */
    enqueue_event(&global_event_queue, sig + 1000);
    global_event_queue.average_size = (global_event_queue.average_size + global_event_queue.size) / 2.0;

    printf("Handler: log_count=%d, overruns=%d, queue_size=%d, max_reached=%d\n",
           global_log_buffer.count, global_log_buffer.overruns,
           global_event_queue.size, global_event_queue.max_size_reached);
}

int main() {
    printf("Demonstrating unsafe circular buffer access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 40; i++) {
        /* Add main program log entries */
        char log_entry[128];
        sprintf(log_entry, "Main program iteration %d", i);
        add_log_entry(&global_log_buffer, log_entry);
        strcpy(global_log_buffer.last_operation, "main_add");

        /* Add events to queue */
        enqueue_event(&global_event_queue, i);
        global_event_queue.average_size = (global_event_queue.average_size + global_event_queue.size) / 2.0;

        /* Process some events (dequeue) */
        if (i % 3 == 0 && global_event_queue.size > 0) {
            global_event_queue.front = (global_event_queue.front + 1) % BUFFER_SIZE;
            global_event_queue.size--;
        }

        printf("Main: log_count=%d, overruns=%d, queue_size=%d, avg_size=%.1f, op=%s\n",
               global_log_buffer.count, global_log_buffer.overruns,
               global_event_queue.size, global_event_queue.average_size,
               global_log_buffer.last_operation);

        usleep(80000);
    }

    return 0;
}