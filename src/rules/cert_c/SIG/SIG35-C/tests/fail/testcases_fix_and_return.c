/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int *global_ptr = NULL;
volatile int safe_value = 42;

void segv_fix_handler(int sig) {
    printf("SIGSEGV handler: Attempting to 'fix' the problem\n");

    /* Misguided attempt to fix the segmentation fault */
    if (global_ptr == NULL) {
        printf("Fixing null pointer by redirecting to safe memory\n");
        global_ptr = &safe_value;
    }

    printf("Problem 'fixed', returning to continue execution (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing exception handler that tries to fix problem and return\n");
    printf("PID: %d\n", getpid());

    signal(SIGSEGV, segv_fix_handler);

    printf("Dereferencing null pointer...\n");
    volatile int value = *global_ptr;

    printf("Value: %d (undefined behavior if printed)\n", value);
    return 0;
}