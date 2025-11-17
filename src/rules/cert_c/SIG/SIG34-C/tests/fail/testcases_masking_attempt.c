/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t mask_attempts = 0;

void masking_handler(int sig) {
    mask_attempts++;
    printf("Signal %d received, attempting masking via signal() (attempt %d)\n", sig, mask_attempts);

    // VIOLATION: Attempting to mask signals using signal() instead of sigprocmask()
    if (mask_attempts % 3 == 1) {
        printf("Attempting to 'mask' by setting to ignore\n");
        if (signal(SIGUSR2, SIG_IGN) == SIG_ERR) {
            printf("Failed to ignore SIGUSR2\n");
        }
    } else if (mask_attempts % 3 == 2) {
        printf("Attempting to 'unmask' by setting handler\n");
        if (signal(SIGUSR2, masking_handler) == SIG_ERR) {
            printf("Failed to set SIGUSR2 handler\n");
        }
    } else {
        printf("Attempting to 'mask' original signal\n");
        if (signal(sig, SIG_IGN) == SIG_ERR) {
            printf("Failed to ignore original signal\n");
        }
    }

    // This approach is fundamentally flawed and creates race conditions
    usleep(2000); // Simulate processing that should be protected

    printf("Masking attempt complete (ineffective and dangerous)\n");
}

int main() {
    printf("SIG34-C VIOLATION: Using signal() for signal masking attempts\n");
    printf("Misguided attempt to mask signals with signal() instead of sigprocmask()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, masking_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 and SIGUSR2 to see flawed masking attempts\n");

    while (mask_attempts < 9) {
        pause();
    }

    printf("Masking attempts completed: %d\n", mask_attempts);
    return 0;
}