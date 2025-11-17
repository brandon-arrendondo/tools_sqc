/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: signal() return value is not checked for SIG_ERR
 */

#include <stdio.h>
#include <signal.h>

void signal_handler(int sig) {
    printf("Signal %d received\n", sig);
}

int main() {
    // VIOLATION: Return value not checked for SIG_ERR
    signal(SIGINT, signal_handler);

    printf("Signal handler supposedly registered\n");

    // Another unchecked signal call
    signal(SIGTERM, SIG_IGN);
    printf("SIGTERM supposedly ignored\n");

    return 0;
}