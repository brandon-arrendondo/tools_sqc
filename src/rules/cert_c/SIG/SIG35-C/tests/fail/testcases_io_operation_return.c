/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *debug_file = NULL;

void io_performing_handler(int sig) {
    printf("Exception handler: Performing I/O operations\n");

    /* Attempt to write to file from signal handler */
    if (debug_file != NULL) {
        fprintf(debug_file, "Exception %d occurred\n", sig);
        fflush(debug_file);
    }

    /* Write to stderr */
    fprintf(stderr, "Signal %d handled via I/O operations\n", sig);

    printf("I/O operations completed, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing I/O operations in exception handler with return\n");
    printf("PID: %d\n", getpid());

    debug_file = fopen("/tmp/claude/debug.log", "w");
    signal(SIGSEGV, io_performing_handler);

    printf("Triggering segmentation fault...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This represents undefined behavior\n");

    if (debug_file) fclose(debug_file);
    return 0;
}