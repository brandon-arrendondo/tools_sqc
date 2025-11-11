/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t recovery_attempted = 0;
volatile sig_atomic_t recovery_successful = 0;

void recovery_handler(int sig) {
    printf("Exception handler: Attempting automatic recovery\n");
    recovery_attempted = 1;

    /* Simulate recovery logic */
    printf("Running recovery procedures...\n");
    printf("Checking system state...\n");
    printf("Attempting to restore normal operation...\n");

    /* Falsely assume recovery was successful */
    recovery_successful = 1;

    printf("Recovery complete, resuming execution (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing automatic recovery attempt with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, recovery_handler);

    printf("Status: recovery_attempted=%d, recovery_successful=%d\n",
           recovery_attempted, recovery_successful);

    printf("Performing operation that will fail...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Post-recovery status: attempted=%d, successful=%d\n",
           recovery_attempted, recovery_successful);
    printf("This represents undefined behavior\n");

    return 0;
}