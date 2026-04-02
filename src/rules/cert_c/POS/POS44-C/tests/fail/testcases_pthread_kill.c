/*
 * Rule: POS44-C
 * Source: testcases
 * Status: FAIL - Should trigger POS44-C violation
 *
 * Using pthread_kill() to terminate threads
 */

#include <pthread.h>
#include <signal.h>

void terminate_thread(pthread_t thread) {
    /* VIOLATION: pthread_kill is unsafe for thread termination */
    pthread_kill(thread, SIGTERM);
}
