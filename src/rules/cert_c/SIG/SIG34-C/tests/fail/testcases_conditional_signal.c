/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t handler_enabled = 1;

void conditional_handler(int sig) {
    signal_count++;
    printf("Signal %d received (count: %d, enabled: %d)\n", sig, signal_count, handler_enabled);

    // VIOLATION: Conditional signal() call within handler based on state
    if (handler_enabled && signal_count % 2 == 0) {
        printf("Disabling handler due to even count\n");
        if (signal(sig, SIG_DFL) == SIG_ERR) {
            printf("Failed to set default handler\n");
        }
        handler_enabled = 0;
    } else if (!handler_enabled && signal_count % 3 == 0) {
        printf("Re-enabling handler due to count divisible by 3\n");
        if (signal(sig, conditional_handler) == SIG_ERR) {
            printf("Failed to re-enable handler\n");
        }
        handler_enabled = 1;
    }

    printf("Handler state modification complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Conditional signal() calls in handler\n");
    printf("Handler modifies its own registration based on state\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, conditional_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 signals to see conditional behavior\n");

    while (signal_count < 12) {
        pause();
    }

    printf("Processed %d signals\n", signal_count);
    return 0;
}