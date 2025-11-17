/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// SAFE: Only volatile sig_atomic_t flags modified in handlers
volatile sig_atomic_t sigusr1_received = 0;
volatile sig_atomic_t sigusr2_received = 0;
volatile sig_atomic_t terminate_requested = 0;
volatile sig_atomic_t handler_call_count = 0;

void flag_only_sigusr1_handler(int sig) {
    // SAFE: Only setting flags, no signal() calls
    sigusr1_received = 1;
    handler_call_count++;
}

void flag_only_sigusr2_handler(int sig) {
    // SAFE: Only setting flags, no signal() calls
    sigusr2_received = 1;
    handler_call_count++;
}

void flag_only_term_handler(int sig) {
    // SAFE: Only setting flags, no signal() calls
    terminate_requested = 1;
    handler_call_count++;
}

int main() {
    struct sigaction sa1, sa2, sa_term;
    printf("SIG34-C COMPLIANT: Signal handlers that only set flags\n");
    printf("No signal() calls whatsoever in any handler\n");
    printf("PID: %d\n", getpid());

    // SAFE: All signal registration done in main thread using sigaction()

    sa1.sa_handler = flag_only_sigusr1_handler;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa1, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    sa2.sa_handler = flag_only_sigusr2_handler;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR2, &sa2, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    sa_term.sa_handler = flag_only_term_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction SIGTERM");
        exit(EXIT_FAILURE);
    }

    printf("All handlers registered safely - they only set flags\n");
    printf("Send SIGUSR1, SIGUSR2, or SIGTERM to test flag-only handling\n");

    // Main loop processes flags set by handlers
    while (!terminate_requested && handler_call_count < 10) {
        if (sigusr1_received) {
            printf("Main thread: SIGUSR1 flag detected, processing...\n");
            sigusr1_received = 0; // Reset flag
        }

        if (sigusr2_received) {
            printf("Main thread: SIGUSR2 flag detected, processing...\n");
            sigusr2_received = 0; // Reset flag
        }

        // Brief pause to check for signals
        usleep(100000); // 100ms
    }

    if (terminate_requested) {
        printf("Termination requested via signal flag\n");
    }

    printf("Safe flag-only signal handling complete: %d handler calls\n", handler_call_count);
    return 0;
}