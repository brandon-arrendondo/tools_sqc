/*
 * Rule: POS44-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS44-C violation
 *
 * Using pthread_cancel() for cooperative termination
 */

#include <pthread.h>

void cancel_thread(pthread_t thread) {
    /* COMPLIANT: pthread_cancel allows cleanup handlers */
    pthread_cancel(thread);
}
