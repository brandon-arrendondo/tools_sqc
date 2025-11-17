/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t handler_calls = 0;

void self_reregistering_handler(int sig) {
    handler_calls++;
    printf("Signal %d received, re-registering self (call %d)\n", sig, handler_calls);

    // VIOLATION: Calling signal() from within signal handler
    if (signal(sig, self_reregistering_handler) == SIG_ERR) {
        printf("Failed to re-register handler\n");
        exit(EXIT_FAILURE);
    }

    printf("Self re-registration complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Signal handler re-registering itself\n");
    printf("Creates race condition between handler entry and signal() call\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGINT, self_reregistering_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Press Ctrl+C to trigger handler self-registration\n");

    while (handler_calls < 5) {
        pause();
    }

    printf("Handler called %d times\n", handler_calls);
    return 0;
}