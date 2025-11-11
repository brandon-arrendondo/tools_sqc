/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG02-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t task_queued = 0;
volatile sig_atomic_t task_completed = 0;
volatile sig_atomic_t queue_size = 0;

void task_queue_handler(int sig) {
    if (sig == SIGUSR1) {
        task_queued = 1;
        queue_size++;
        printf("Task queued signal received (queue size: %d)\n", queue_size);
    } else if (sig == SIGUSR2) {
        task_completed = 1;
        if (queue_size > 0) queue_size--;
        printf("Task completed signal received (queue size: %d)\n", queue_size);
    }
}

int main() {
    printf("Using signals for normal task queue management (BAD)\n");

    signal(SIGUSR1, task_queue_handler);
    signal(SIGUSR2, task_queue_handler);

    pid_t task_producer = fork();
    if (task_producer == 0) {
        printf("Task Producer: Adding tasks to queue\n");
        for (int i = 0; i < 5; i++) {
            sleep(1);
            printf("Task Producer: Adding task %d\n", i + 1);
            kill(getppid(), SIGUSR1);
        }
        exit(0);
    }

    pid_t task_worker = fork();
    if (task_worker == 0) {
        sleep(3);
        printf("Task Worker: Starting to process tasks\n");
        for (int i = 0; i < 5; i++) {
            sleep(2);
            printf("Task Worker: Completed task %d\n", i + 1);
            kill(getppid(), SIGUSR2);
        }
        exit(0);
    }

    // Queue manager
    printf("Queue Manager: Monitoring task queue\n");
    int events = 0;
    while (events < 10) {
        pause();
        if (task_queued) {
            printf("Queue Manager: Task added to queue\n");
            task_queued = 0;
            events++;
        }
        if (task_completed) {
            printf("Queue Manager: Task completed and removed\n");
            task_completed = 0;
            events++;
        }
    }

    wait(NULL);
    wait(NULL);
    printf("Task queue management complete\n");
    return 0;
}