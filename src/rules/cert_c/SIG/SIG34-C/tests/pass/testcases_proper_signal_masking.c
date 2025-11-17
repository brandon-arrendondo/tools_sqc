/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t masked_signals = 0;

void masking_aware_handler(int sig) {
    masked_signals++;
    printf("Masking-aware handler for signal %d (count: %d)\n", sig, masked_signals);

    // SAFE: No signal disposition changes in handler
    // Proper masking is handled by sigaction() setup, not by signal() calls

    printf("Handler executes with proper signal masking (set by sigaction)\n");

    // Handler is automatically protected by signal mask set in main()
    // No need for signal() calls to attempt masking
}

int main() {
    struct sigaction sa;
    sigset_t block_set;
    printf("SIG34-C COMPLIANT: Proper signal masking with sigaction()\n");
    printf("Using sigprocmask() and sa_mask for safe signal masking\n");
    printf("PID: %d\n", getpid());

    // SAFE: Proper signal masking using sigprocmask() and sigaction()

    // Set up signal mask to block SIGUSR2 during SIGUSR1 handling
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR2); // Block SIGUSR2 during handler execution
    sigaddset(&sa.sa_mask, SIGTERM); // Also block SIGTERM during handler

    sa.sa_handler = masking_aware_handler;
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    // Set up different masking for SIGUSR2
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1); // Block SIGUSR1 during SIGUSR2 handling

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    // Demonstrate proper signal masking in main thread
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGTERM);

    printf("Temporarily blocking SIGTERM in main thread\n");
    if (sigprocmask(SIG_BLOCK, &block_set, NULL) == -1) {
        perror("sigprocmask block");
        exit(EXIT_FAILURE);
    }

    printf("Signal masking configured safely - no signal() calls needed\n");
    printf("Send SIGUSR1 and SIGUSR2 to test proper masking\n");

    while (masked_signals < 8) {
        pause();
    }

    printf("Unblocking SIGTERM\n");
    if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) == -1) {
        perror("sigprocmask unblock");
        exit(EXIT_FAILURE);
    }

    printf("Proper signal masking demonstration complete: %d signals\n", masked_signals);
    return 0;
}