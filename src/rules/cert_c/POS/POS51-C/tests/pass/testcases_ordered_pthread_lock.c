/*
 * Rule: POS51-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS51-C violation
 *
 * Ordered pthread mutex locking with comparison
 */

#include <pthread.h>

void ordered_pthread_lock(pthread_mutex_t *m1, pthread_mutex_t *m2) {
    /* COMPLIANT: ordering based on pointer comparison */
    if (m1 < m2) {
        pthread_mutex_lock(m1);
        pthread_mutex_lock(m2);
    } else {
        pthread_mutex_lock(m2);
        pthread_mutex_lock(m1);
    }
    pthread_mutex_unlock(m1);
    pthread_mutex_unlock(m2);
}
