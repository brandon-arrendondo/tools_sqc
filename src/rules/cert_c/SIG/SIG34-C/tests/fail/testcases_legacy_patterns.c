/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t legacy_calls = 0;

void legacy_signal_handler(int sig) {
    legacy_calls++;
    printf("Legacy handler called for signal %d (call %d)\n", sig, legacy_calls);

    // VIOLATION: Legacy signal handling patterns with signal() in handlers
    // This mimics old-style signal handling that was common before sigaction()

    printf("Using legacy signal() pattern (unreliable and dangerous)\n");

    // Classic legacy pattern: immediately re-register handler
    if (signal(sig, legacy_signal_handler) == SIG_ERR) {
        printf("Legacy pattern failed: couldn't re-register handler\n");
        exit(EXIT_FAILURE);
    }

    printf("Legacy re-registration complete\n");

    // Legacy pattern: manual signal management
    if (legacy_calls % 2 == 0) {
        printf("Legacy pattern: manually managing SIGCHLD\n");
        if (signal(SIGCHLD, legacy_signal_handler) == SIG_ERR) {
            printf("Legacy SIGCHLD management failed\n");
        }
    }

    // Legacy pattern: signal() for cleanup
    if (legacy_calls % 3 == 0) {
        printf("Legacy pattern: signal() cleanup operations\n");
        signal(SIGPIPE, SIG_IGN);   // Ignore return value (bad practice)
        signal(SIGTERM, SIG_DFL);   // Reset to default (bad practice)
    }

    // Legacy pattern: conditional signal() based on signal count
    if (legacy_calls > 5) {
        printf("Legacy pattern: disabling handler after threshold\n");
        if (signal(sig, SIG_DFL) == SIG_ERR) {
            printf("Legacy disable pattern failed\n");
        }
    }

    // Legacy pattern: signal() without error checking (very bad)
    if (legacy_calls % 4 == 0) {
        printf("Legacy pattern: signal() without error checking\n");
        signal(SIGQUIT, legacy_signal_handler); // No error check!
    }

    printf("Legacy signal() pattern execution complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Legacy signal handling patterns with signal() in handlers\n");
    printf("Demonstrates old, unreliable signal() patterns that should be avoided\n");
    printf("PID: %d\n", getpid());

    // Legacy pattern: simple signal() registration
    if (signal(SIGUSR1, legacy_signal_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to see legacy signal() patterns\n");

    while (legacy_calls < 8) {
        pause();
    }

    printf("Legacy pattern executions: %d\n", legacy_calls);
    return 0;
}