/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void error_handler(int sig) {
    const char *msg;
    switch (sig) {
        case SIGFPE:
            msg = "ERROR: Floating point exception detected\n";
            break;
        case SIGSEGV:
            msg = "ERROR: Segmentation fault detected\n";
            break;
        case SIGILL:
            msg = "ERROR: Illegal instruction detected\n";
            break;
        default:
            msg = "ERROR: Unknown signal received\n";
            break;
    }
    write(STDERR_FILENO, msg, strlen(msg));
    _exit(EXIT_FAILURE);
}

int main() {
    printf("Using signals only for error handling (GOOD)\n");

    signal(SIGFPE, error_handler);
    signal(SIGSEGV, error_handler);
    signal(SIGILL, error_handler);

    printf("Normal program flow using standard mechanisms\n");

    int value = 42;
    printf("Processing value: %d\n", value);

    printf("Program completed normally\n");
    return 0;
}