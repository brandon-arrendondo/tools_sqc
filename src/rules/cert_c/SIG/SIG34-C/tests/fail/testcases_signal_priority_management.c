/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t priority_operations = 0;

void priority_handler(int sig) {
    priority_operations++;
    printf("Priority handler for signal %d (operation %d)\n", sig, priority_operations);

    // VIOLATION: Attempting priority management with signal() calls
    printf("Managing signal priorities with signal() (incorrect approach)\n");

    // Misguided attempt to create signal priority levels
    if (sig == SIGUSR1) {
        printf("High priority signal: setting up lower priority handlers\n");

        // Incorrectly trying to create priority by registration order
        if (signal(SIGUSR2, priority_handler) == SIG_ERR) {
            printf("Failed to set medium priority handler\n");
        }
        if (signal(SIGTERM, priority_handler) == SIG_ERR) {
            printf("Failed to set low priority handler\n");
        }

        // Re-register self as highest priority
        if (signal(SIGUSR1, priority_handler) == SIG_ERR) {
            printf("Failed to maintain high priority\n");
        }
    } else if (sig == SIGUSR2) {
        printf("Medium priority signal: managing priorities\n");

        // Wrong approach to priority management
        if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
            printf("Failed to deprioritize SIGTERM\n");
        }
        if (signal(SIGUSR1, priority_handler) == SIG_ERR) {
            printf("Failed to ensure high priority remains\n");
        }
    } else if (sig == SIGTERM) {
        printf("Low priority signal: attempting priority boost\n");

        // Incorrect priority boosting attempt
        if (signal(SIGTERM, priority_handler) == SIG_ERR) {
            printf("Failed to boost own priority\n");
        }
        if (signal(SIGUSR2, SIG_DFL) == SIG_ERR) {
            printf("Failed to demote medium priority\n");
        }
    }

    // Attempt to maintain priority ordering (which signal() cannot provide)
    printf("Priority management operations complete (ineffective)\n");
}

int main() {
    printf("SIG34-C VIOLATION: Signal priority management using signal()\n");
    printf("Handler incorrectly attempts to manage signal priorities with signal() calls\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, priority_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 (high), SIGUSR2 (med), SIGTERM (low) to see priority attempts\n");

    while (priority_operations < 9) {
        pause();
    }

    printf("Priority management attempts: %d (all ineffective)\n", priority_operations);
    return 0;
}