/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: One thread frees memory while another tries to access it
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int *shared_ptr;

void *worker_thread(void *arg) {
    sleep(1);  // Wait a bit
    // BUG: Access potentially freed memory
    if (shared_ptr != NULL) {
        printf("Worker sees: %d\n", *shared_ptr);
    }
    return NULL;
}

int main() {
    shared_ptr = malloc(sizeof(int));
    if (shared_ptr == NULL) {
        return -1;
    }

    *shared_ptr = 555;

    pthread_t thread;
    pthread_create(&thread, NULL, worker_thread, NULL);

    // Free while other thread might access
    free(shared_ptr);
    shared_ptr = NULL;

    pthread_join(thread, NULL);
    return 0;
}