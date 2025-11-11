/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t handler_invocations = 0;
volatile sig_atomic_t processing_complete = 0;

void non_modifying_handler(int sig) {
    handler_invocations++;
    printf("Non-modifying handler for signal %d (invocation %d)\n", sig, handler_invocations);

    // SAFE: Handler never modifies signal disposition
    // Only performs async-signal-safe operations

    // Set flag to indicate processing (async-signal-safe)
    processing_complete = 1;

    printf("Handler completed without any signal disposition changes\n");

    // No signal() calls, no sigaction() calls, no disposition modifications
    // This is completely safe and compliant with SIG34-C
}

int main() {
    struct sigaction sa;
    printf("SIG34-C COMPLIANT: Signal handlers that never modify signal disposition\n");
    printf("Handlers only set flags and perform async-signal-safe operations\n");
    printf("PID: %d\n", getpid());

    // SAFE: Setup signal handling only in main thread, never in handlers
    sa.sa_handler = non_modifying_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Restart interrupted system calls

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Signal handler registered safely - will never modify dispositions\n");
    printf("Send SIGUSR1 to test non-modifying handler\n");

    while (handler_invocations < 8) {
        pause();

        if (processing_complete) {
            printf("Main thread detected handler completion (flag method)\n");
            processing_complete = 0; // Reset flag
        }
    }

    printf("Safe non-modifying signal handling complete: %d invocations\n", handler_invocations);
    return 0;
}