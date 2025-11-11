/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t interrupt_count = 0;
volatile sig_atomic_t should_exit = 0;

void minimal_handler(int sig) {
    // Compliant: Minimal handler with only atomic operations
    if (sig == SIGINT) {
        interrupt_count++;
        if (interrupt_count >= 3) {
            should_exit = 1;
        }
    }
}

int main() {
    struct sigaction sa;

    sa.sa_handler = minimal_handler;
    sigemptyset(&sa.sa_mask);

    // Compliant: Mask SIGINT during its own handler execution
    sigaddset(&sa.sa_mask, SIGINT);

    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Press Ctrl+C three times to exit\n");
    printf("Handler is minimal and cannot be interrupted\n");

    while (!should_exit) {
        printf("Working... (interrupts: %d/3)\n", interrupt_count);

        // Complex work in main thread is safe
        for (int i = 0; i < 10; i++) {
            printf("  Main thread work step %d\n", i + 1);
            sleep(1);

            // Check exit condition regularly
            if (should_exit) {
                printf("\nExit signal received, shutting down safely\n");
                break;
            }
        }

        if (interrupt_count > 0 && interrupt_count < 3) {
            printf("Received %d interrupts, need %d more to exit\n",
                   interrupt_count, 3 - interrupt_count);
        }
    }

    printf("Clean shutdown completed\n");
    return 0;
}