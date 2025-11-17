/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t state = 0;

void state_handler(int sig) {
    state = 1;
    printf("State changed to: %d\n", state);
}

int main() {
    printf("FAIL: No verification of signal handler persistence\n");

    /* Set handler without verifying it remains set */
    signal(SIGUSR1, state_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 to change state, then SIGUSR1 again\n");

    /* Wait for first signal */
    while (state == 0) {
        pause();
    }

    printf("State is now %d\n", state);

    /* Reset state and assume handler is still active */
    state = 0;
    printf("Reset state to 0, waiting for another signal...\n");

    /* No verification that handler is still registered */
    while (state == 0) {
        pause();
    }

    printf("Final state: %d\n", state);
    return 0;
}