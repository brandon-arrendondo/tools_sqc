/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void signal_mask_handler(int sig) {
    sigset_t new_mask, old_mask;

    printf("Exception handler: Modifying signal mask\n");

    /* Attempt to modify signal mask */
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGFPE);
    sigaddset(&new_mask, SIGSEGV);

    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) == 0) {
        printf("Signal mask modified successfully\n");
    } else {
        printf("Failed to modify signal mask\n");
    }

    printf("Signal mask operations complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing signal mask modification with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, signal_mask_handler);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This represents undefined behavior\n");
    return 0;
}