/*
 * Rule: POS04-C
 * Source: testcases
 * Status: FAIL - Should trigger POS04-C violation
 *
 * Using PTHREAD_MUTEX_NORMAL type (no error checking)
 */

#include <pthread.h>

void create_normal_mutex(void) {
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    /* VIOLATION: NORMAL type does not detect deadlock */
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&mutex, &attr);
}
