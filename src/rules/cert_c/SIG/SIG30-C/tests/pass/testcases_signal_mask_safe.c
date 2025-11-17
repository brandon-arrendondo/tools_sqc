/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t interrupt_count = 0;
volatile sig_atomic_t cleanup_requested = 0;

void interrupt_handler(int sig) {
    // SAFE: Only modify sig_atomic_t variables
    interrupt_count++;
}

void cleanup_handler(int sig) {
    // SAFE: Set cleanup flag
    cleanup_requested = 1;
}

int main() {
    printf("Demonstrating safe signal handling with proper signal masking\n");
    printf("PID: %d\n", getpid());

    // Set up signal handlers
    signal(SIGUSR1, interrupt_handler);
    signal(SIGTERM, cleanup_handler);

    printf("SIGUSR1 will increment counter (safe atomic operation)\n");
    printf("SIGTERM will request cleanup (safe flag setting)\n");

    // Main processing loop
    while (!cleanup_requested) {
        // Block signals during critical section
        sigset_t mask, oldmask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);

        // Block SIGUSR1 during counter reading
        sigprocmask(SIG_BLOCK, &mask, &oldmask);

        // Safe to read counter now that signals are blocked
        int current_count = interrupt_count;

        // Restore signal mask
        sigprocmask(SIG_SETMASK, &oldmask, NULL);

        printf("Interrupt count: %d\n", current_count);

        // Process other work
        printf("Doing work... (send SIGUSR1 to interrupt, SIGTERM to exit)\n");
        sleep(2);
    }

    printf("Cleanup requested, exiting safely...\n");
    printf("Final interrupt count: %d\n", (int)interrupt_count);

    return 0;
}