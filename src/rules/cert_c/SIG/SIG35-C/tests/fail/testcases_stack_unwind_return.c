/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

jmp_buf exception_context;
volatile sig_atomic_t unwind_attempted = 0;

void stack_unwinding_handler(int sig) {
    printf("Exception handler: Attempting stack unwinding\n");
    unwind_attempted = 1;

    /* Misguided attempt to unwind the stack and return */
    printf("Performing 'stack cleanup' before return\n");

    /* This is still a violation even with attempted cleanup */
    printf("Unwinding complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

void nested_function() {
    printf("In nested function, triggering exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;
    printf("This should not print due to exception\n");
}

int main() {
    printf("Testing stack unwinding attempt with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, stack_unwinding_handler);

    if (setjmp(exception_context) == 0) {
        printf("Calling nested function...\n");
        nested_function();
    } else {
        printf("Returned from longjmp (not reached in this case)\n");
    }

    printf("Undefined behavior if this executes\n");
    return 0;
}