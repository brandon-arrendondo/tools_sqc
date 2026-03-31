/*
 * Rule: POS53-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS53-C violation
 *
 * Condition variable consistently used with same mutex
 */

#include <pthread.h>

pthread_cond_t cond;
pthread_mutex_t mutex;

void producer(void) {
    pthread_mutex_lock(&mutex);
    /* COMPLIANT: cond always used with same mutex */
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void consumer(void) {
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
}
