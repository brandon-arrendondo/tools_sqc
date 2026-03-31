/*
 * Rule: POS48-C
 * Source: testcases
 * Status: FAIL - Should trigger POS48-C violation
 *
 * Shared data access after mutex destroy without waiting
 */

#include <pthread.h>

pthread_mutex_t mutex;
int shared_data;

void cleanup_no_wait(void) {
    /* VIOLATION: destroy without waiting for all threads */
    pthread_mutex_destroy(&mutex);
    shared_data = 0;
}
