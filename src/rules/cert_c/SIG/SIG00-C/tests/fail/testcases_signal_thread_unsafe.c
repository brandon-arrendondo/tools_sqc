/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

volatile sig_atomic_t thread_signals = 0;
pthread_t worker_thread;

// Global thread-unsafe data
char global_buffer[1024];
int global_index = 0;

void* worker_function(void* arg) {
    while (1) {
        // Worker thread modifies global data
        if (global_index < sizeof(global_buffer) - 10) {
            sprintf(global_buffer + global_index, "T%d ", (int)pthread_self() % 1000);
            global_index += 4;
        } else {
            global_index = 0;
            memset(global_buffer, 0, sizeof(global_buffer));
        }

        usleep(100000); // 100ms
    }
    return NULL;
}

void thread_unsafe_handler(int sig) {
    thread_signals++;

    printf("Handler: Signal %d in thread %lu\n", sig, pthread_self());

    // Violation: Accessing thread-unsafe data without proper masking
    // Signal can arrive in any thread and corrupt shared state
    if (global_index < sizeof(global_buffer) - 20) {
        sprintf(global_buffer + global_index, "S%d-%d ", sig, thread_signals);
        global_index += strlen(global_buffer + global_index);
    }

    // More thread-unsafe operations
    printf("Handler: Global buffer content: %.100s...\n", global_buffer);

    // Simulate complex signal handling
    for (int i = 0; i < 5; i++) {
        if (global_index < sizeof(global_buffer) - 10) {
            global_buffer[global_index++] = 'A' + i;
            global_buffer[global_index] = '\0';
        }

        // Create race condition window
        usleep(20000);
    }

    printf("Handler: Modified global data, index now %d\n", global_index);
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = thread_unsafe_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Signal handling without thread safety consideration
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    // Create worker thread
    if (pthread_create(&worker_thread, NULL, worker_function, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Worker thread created, send signals to cause thread races\n");

    while (1) {
        printf("Main: Thread signals received: %d\n", thread_signals);
        printf("Main: Global buffer index: %d\n", global_index);
        printf("Main: Buffer sample: %.50s...\n", global_buffer);

        // Check for obvious corruption
        for (int i = 0; i < global_index; i++) {
            if (global_buffer[i] == '\0' && i < global_index - 1) {
                printf("Main: ERROR - Null byte found at position %d!\n", i);
                break;
            }
        }

        sleep(2);
    }

    pthread_cancel(worker_thread);
    pthread_join(worker_thread, NULL);
    return 0;
}