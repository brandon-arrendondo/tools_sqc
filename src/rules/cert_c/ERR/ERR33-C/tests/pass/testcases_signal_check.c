/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: signal() return value is properly checked for errors
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void signal_handler(int sig) {
    printf("Signal %d received\n", sig);
}

int main() {
    // Register signal handler and check for errors
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Failed to register signal handler\n");
        return 1;
    }

    printf("Signal handler registered successfully\n");

    // Reset signal to default and check for errors
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        fprintf(stderr, "Failed to reset signal handler\n");
        return 1;
    }

    printf("Signal handler reset successfully\n");
    return 0;
}