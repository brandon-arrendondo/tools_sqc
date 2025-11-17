/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t helper_called = 0;

void exception_helper(int sig_num) {
    helper_called = 1;
    printf("Helper function called for signal %d\n", sig_num);
    printf("Performing auxiliary exception handling\n");
    /* Helper function completes normally */
}

void function_calling_handler(int sig) {
    printf("Exception handler: Calling helper function\n");

    /* Call another function from the handler */
    exception_helper(sig);

    printf("Helper function completed, returning from handler (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing exception handler that calls functions and returns\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, function_calling_handler);

    printf("Helper called status: %d\n", helper_called);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Helper called after exception: %d\n", helper_called);
    printf("This represents undefined behavior\n");

    return 0;
}