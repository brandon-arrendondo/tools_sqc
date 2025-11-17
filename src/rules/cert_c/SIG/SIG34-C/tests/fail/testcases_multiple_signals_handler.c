/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t sigusr1_count = 0;
volatile sig_atomic_t sigusr2_count = 0;

void multi_signal_handler(int sig) {
    printf("Handler called for signal %d\n", sig);

    if (sig == SIGUSR1) {
        sigusr1_count++;
        printf("SIGUSR1 count: %d\n", sigusr1_count);

        // VIOLATION: Calling signal() for different signal from within handler
        if (signal(SIGUSR2, multi_signal_handler) == SIG_ERR) {
            printf("Failed to register SIGUSR2 handler\n");
        }
    } else if (sig == SIGUSR2) {
        sigusr2_count++;
        printf("SIGUSR2 count: %d\n", sigusr2_count);

        // VIOLATION: Calling signal() for different signal from within handler
        if (signal(SIGUSR1, multi_signal_handler) == SIG_ERR) {
            printf("Failed to register SIGUSR1 handler\n");
        }
    }

    printf("Signal handler modifications complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Handler modifying other signal handlers\n");
    printf("Multiple signals competing for signal() calls\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, multi_signal_handler) == SIG_ERR) {
        perror("signal SIGUSR1");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 and SIGUSR2 to see cross-registration\n");

    while (sigusr1_count + sigusr2_count < 8) {
        pause();
    }

    printf("Total signals processed: %d\n", sigusr1_count + sigusr2_count);
    return 0;
}