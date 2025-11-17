/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t reset_count = 0;

void resetting_handler(int sig) {
    reset_count++;
    printf("Signal %d received, attempting to reset disposition (count: %d)\n", sig, reset_count);

    // VIOLATION: Resetting signal disposition using signal() from within handler
    if (reset_count % 2 == 0) {
        printf("Resetting to default disposition\n");
        if (signal(sig, SIG_DFL) == SIG_ERR) {
            printf("Failed to reset to default\n");
        }
    } else {
        printf("Resetting to ignore disposition\n");
        if (signal(sig, SIG_IGN) == SIG_ERR) {
            printf("Failed to reset to ignore\n");
        }
    }

    // Race condition: another signal could arrive here before disposition change
    usleep(1000); // Simulate processing time

    printf("Signal disposition reset complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Resetting signal disposition in handler\n");
    printf("Handler alternates between default and ignore using signal()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR2, resetting_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR2 to see disposition resets\n");

    while (reset_count < 6) {
        pause();
    }

    printf("Reset operations completed: %d\n", reset_count);
    return 0;
}