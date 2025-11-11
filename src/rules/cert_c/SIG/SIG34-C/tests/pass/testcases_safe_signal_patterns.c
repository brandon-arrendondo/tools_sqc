/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t pattern_count = 0;
volatile sig_atomic_t cleanup_phase = 0;

void safe_pattern_handler(int sig) {
    pattern_count++;

    // SAFE: Handler follows all safe signal handling patterns
    // 1. Only async-signal-safe operations
    // 2. No signal() calls
    // 3. No sigaction() calls
    // 4. Only modify volatile sig_atomic_t variables
    // 5. Minimal processing

    if (sig == SIGUSR1) {
        // Pattern: Set flag for main thread to process
        cleanup_phase = 1;
    } else if (sig == SIGUSR2) {
        // Pattern: Different flag for different processing
        cleanup_phase = 2;
    }

    // Pattern: Keep handlers simple and fast
    // No complex logic, no system calls, no signal modifications
}

int main() {
    struct sigaction sa;
    printf("SIG34-C COMPLIANT: Safe signal handler patterns\n");
    printf("Demonstrates all safe practices without signal() calls\n");
    printf("PID: %d\n", getpid());

    // SAFE: Pattern 1 - Use sigaction() exclusively for setup
    sa.sa_handler = safe_pattern_handler;
    sigemptyset(&sa.sa_mask);

    // SAFE: Pattern 2 - Block related signals during handler execution
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);

    sa.sa_flags = SA_RESTART; // Pattern 3 - Use appropriate flags

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("Safe signal patterns implemented:\n");
    printf("- sigaction() for all registration\n");
    printf("- Handlers only set flags\n");
    printf("- Main thread does all complex processing\n");
    printf("- Proper signal masking\n");
    printf("- No signal() calls anywhere\n");

    printf("Send SIGUSR1 and SIGUSR2 to test safe patterns\n");

    // SAFE: Pattern 4 - Main thread handles complex logic
    while (pattern_count < 8) {
        // Pattern 5 - Check flags and process accordingly
        if (cleanup_phase == 1) {
            printf("Main thread: Processing SIGUSR1 cleanup request\n");
            cleanup_phase = 0;
        } else if (cleanup_phase == 2) {
            printf("Main thread: Processing SIGUSR2 operation request\n");
            cleanup_phase = 0;
        }

        // Pattern 6 - Use appropriate waiting mechanism
        pause(); // Wait for signals
    }

    printf("All safe signal patterns demonstrated successfully\n");
    printf("Total pattern executions: %d\n", pattern_count);
    return 0;
}