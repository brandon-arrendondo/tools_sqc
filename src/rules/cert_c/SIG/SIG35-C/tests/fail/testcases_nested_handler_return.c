/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t handler_depth = 0;

void nested_exception_handler(int sig) {
    handler_depth++;
    printf("Nested handler level %d: Signal %d\n", handler_depth, sig);

    if (handler_depth > 1) {
        printf("Nested exception detected, returning (violates SIG35-C)\n");
        handler_depth--;
        return; /* VIOLATION: returning from computational exception handler */
    }

    printf("First level handler, triggering another exception...\n");
    /* Trigger another exception while in handler */
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This should not execute\n");
    handler_depth--;
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing nested exception handlers that return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, nested_exception_handler);
    signal(SIGSEGV, nested_exception_handler);

    printf("Handler depth: %d\n", handler_depth);

    printf("Triggering first exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Handler depth after exceptions: %d\n", handler_depth);
    printf("This represents undefined behavior\n");

    return 0;
}