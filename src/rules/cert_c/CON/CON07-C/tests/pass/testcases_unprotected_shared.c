/*
 * Rule: CON07-C
 * Source: testcases
 * Status: PASS - Known limitation: pattern not detected
 * TODO: Move to fail/ when implemented (see PLAN.md)
 */

#include <pthread.h>

int shared_counter;
pthread_mutex_t lock;

/* Accessing shared data without holding lock */
void unsafe_increment(void) {
    shared_counter++;
}

/* Reading shared data without lock */
int unsafe_read(void) {
    return shared_counter;
}
