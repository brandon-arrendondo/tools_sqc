/*
 * Rule: POS04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS04-C violation
 *
 * Using PTHREAD_MUTEX_ERRORCHECK type
 */

#include <pthread.h>

void create_errorcheck_mutex(void) {
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    /* COMPLIANT: ERRORCHECK detects deadlock */
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attr);
}
