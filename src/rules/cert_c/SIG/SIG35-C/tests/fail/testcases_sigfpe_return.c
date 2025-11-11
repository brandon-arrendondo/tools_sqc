/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t fpe_occurred = 0;

void fpe_handler(int sig) {
    fpe_occurred = 1;
    printf("SIGFPE handler: Floating point exception caught\n");
    printf("Attempting to continue execution (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing SIGFPE handler return violation\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, fpe_handler);

    printf("Triggering division by zero...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This causes undefined behavior if reached\n");
    return 0;
}