/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t registration_count = 0;

void atomic_safe_handler(int sig) {
    registration_count++;

    // SAFE: Handler performs no signal registration
    // All signal registration is done atomically outside signal context
    printf("Atomic-safe handler for signal %d (count: %d)\n", sig, registration_count);

    // Handler only sets flags and does minimal processing
    // No signal() calls, no sigaction() calls
}

int main() {
    struct sigaction sa, old_sa;
    sigset_t old_mask, block_mask;
    printf("SIG34-C COMPLIANT: Atomic signal handler registration outside signal context\n");
    printf("All signal modifications done in main thread with proper atomicity\n");
    printf("PID: %d\n", getpid());

    // SAFE: Atomic signal handler registration
    printf("Performing atomic signal registration sequence\n");

    // Step 1: Block signals during registration to ensure atomicity
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGUSR1);
    sigaddset(&block_mask, SIGUSR2);
    sigaddset(&block_mask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &block_mask, &old_mask) == -1) {
        perror("sigprocmask block");
        exit(EXIT_FAILURE);
    }

    // Step 2: Atomically register handlers while signals are blocked
    sa.sa_handler = atomic_safe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

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

    printf("All handlers registered atomically while signals blocked\n");

    // Step 3: Atomically unblock signals to enable handlers
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) == -1) {
        perror("sigprocmask restore");
        exit(EXIT_FAILURE);
    }

    printf("Signals unblocked - handlers now active\n");
    printf("Send SIGUSR1, SIGUSR2, or SIGTERM to test atomic registration\n");

    while (registration_count < 8) {
        pause();
    }

    // SAFE: Atomic cleanup - block signals again for safe cleanup
    printf("Performing atomic cleanup\n");

    if (sigprocmask(SIG_BLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask block cleanup");
        exit(EXIT_FAILURE);
    }

    // Atomically restore default handlers
    sa.sa_handler = SIG_DFL;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) == -1) {
        perror("sigprocmask restore cleanup");
        exit(EXIT_FAILURE);
    }

    printf("Atomic signal registration demonstration complete: %d signals\n", registration_count);
    return 0;
}