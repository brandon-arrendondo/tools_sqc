/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>

#define MAX_BACKTRACE 20

void debug_operation_handler(int sig) {
    printf("Exception handler: Performing debugging operations\n");

    /* Attempt to print stack trace */
    void *backtrace_buffer[MAX_BACKTRACE];
    int backtrace_size = backtrace(backtrace_buffer, MAX_BACKTRACE);

    printf("Stack trace (%d frames):\n", backtrace_size);
    char **symbols = backtrace_symbols(backtrace_buffer, backtrace_size);

    if (symbols != NULL) {
        for (int i = 0; i < backtrace_size; i++) {
            printf("  [%d] %s\n", i, symbols[i]);
        }
        free(symbols);
    } else {
        printf("  Failed to get symbols\n");
    }

    /* Print debugging information */
    printf("Debug info: PID=%d, Signal=%d\n", getpid(), sig);
    printf("Debug: Stack pointer approximately at %p\n", &backtrace_size);

    printf("Debugging operations complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing debugging operations in exception handler with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGSEGV, debug_operation_handler);

    printf("Dereferencing null pointer for debugging...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This represents undefined behavior\n");
    return 0;
}