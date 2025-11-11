/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct shared_data {
    volatile sig_atomic_t counter;
    volatile sig_atomic_t status;
    volatile sig_atomic_t last_signal;
};

struct shared_data global_data = {0, 1, 0};

void shared_data_handler(int sig) {
    printf("Exception handler: Accessing shared data\n");

    /* Modify shared data structure */
    global_data.counter++;
    global_data.status = 0; /* Error state */
    global_data.last_signal = sig;

    printf("Shared data updated: counter=%d, status=%d, signal=%d\n",
           global_data.counter, global_data.status, global_data.last_signal);

    printf("Shared data access complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing shared data access in exception handler with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, shared_data_handler);

    printf("Initial shared data: counter=%d, status=%d, signal=%d\n",
           global_data.counter, global_data.status, global_data.last_signal);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Final shared data: counter=%d, status=%d, signal=%d\n",
           global_data.counter, global_data.status, global_data.last_signal);
    printf("This represents undefined behavior\n");

    return 0;
}