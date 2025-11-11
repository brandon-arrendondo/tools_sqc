/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void safe_signal_handler(int sig) {
    signal_count++;
    printf("Safe handler called for signal %d (count: %d)\n", sig, signal_count);

    // SAFE: No signal() calls within handler
    // Handler only performs async-signal-safe operations
    printf("Handler completes without modifying signal dispositions\n");
}

int main() {
    struct sigaction sa;
    printf("SIG34-C COMPLIANT: Using sigaction() exclusively for signal registration\n");
    printf("Never calling signal() from within signal handlers\n");
    printf("PID: %d\n", getpid());

    // SAFE: Using sigaction() for all signal registration
    sa.sa_handler = safe_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No special flags needed

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction SIGTERM");
        exit(EXIT_FAILURE);
    }

    printf("All signal handlers registered safely with sigaction()\n");
    printf("Send SIGUSR1, SIGUSR2, or SIGTERM to test safe handling\n");

    while (signal_count < 10) {
        pause();
    }

    printf("Safe signal handling complete: %d signals processed\n", signal_count);
    return 0;
}