/*
 * Rule: POS53-C
 * Source: testcases
 * Status: FAIL - Should trigger POS53-C violation
 *
 * Same condition variable used with different mutexes
 */

#include <pthread.h>

pthread_cond_t cond;
pthread_mutex_t mutex_a;
pthread_mutex_t mutex_b;

void wait_a(void) {
    pthread_mutex_lock(&mutex_a);
    /* VIOLATION: cond used with mutex_a here and mutex_b elsewhere */
    pthread_cond_wait(&cond, &mutex_a);
    pthread_mutex_unlock(&mutex_a);
}

void wait_b(void) {
    pthread_mutex_lock(&mutex_b);
    pthread_cond_wait(&cond, &mutex_b);
    pthread_mutex_unlock(&mutex_b);
}
