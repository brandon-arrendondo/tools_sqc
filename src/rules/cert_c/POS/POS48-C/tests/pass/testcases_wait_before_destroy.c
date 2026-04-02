/*
 * Rule: POS48-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS48-C violation
 *
 * Wait for all threads before destroying mutex
 */

#include <pthread.h>

pthread_mutex_t mutex;
pthread_t threads[4];

void cleanup_with_wait(void) {
    /* COMPLIANT: wait for threads then destroy */
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
}
