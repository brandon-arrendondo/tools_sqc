/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t handler_calls = 0;

void signal_handler(int sig) {
    handler_calls++;
    printf("Handler called %d times\n", handler_calls);
    /* Assumes signal() behavior is consistent across platforms - VIOLATION */
}

int main() {
    printf("FAIL: Assuming signal() behavior is consistent across platforms\n");

    /* This assumes signal() will work the same on all systems */
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d - Press Ctrl+C multiple times\n", getpid());
    printf("Code assumes handler will persist on all platforms\n");

    /* Loop assumes handler will always be called */
    while (handler_calls < 3) {
        pause();
    }

    printf("Expected 3 calls, got %d\n", handler_calls);
    return 0;
}