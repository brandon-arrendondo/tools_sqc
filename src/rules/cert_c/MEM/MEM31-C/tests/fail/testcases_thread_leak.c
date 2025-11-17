/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Thread-local memory allocated but not freed when thread exits
 */

#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg) {
    char *thread_buffer = malloc(512);
    if (thread_buffer == NULL) {
        return NULL;
    }

    thread_buffer[0] = 'T';

    // Simulate work
    for (int i = 0; i < 1000; i++) {
        thread_buffer[i % 512] = 'X';
    }

    // Thread exits without freeing buffer - MEMORY LEAK
    pthread_exit(NULL);
}

void create_thread() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_function, NULL);
    pthread_join(thread, NULL);
}