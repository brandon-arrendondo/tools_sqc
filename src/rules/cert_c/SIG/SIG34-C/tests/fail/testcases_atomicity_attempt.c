/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t atomicity_attempts = 0;

void atomicity_attempting_handler(int sig) {
    atomicity_attempts++;
    printf("Atomicity handler for signal %d (attempt %d)\n", sig, atomicity_attempts);

    // VIOLATION: Attempting signal() atomicity without proper protection
    printf("Attempting 'atomic' signal() operations (incorrectly)\n");

    // Misguided attempt at atomicity by rapid signal() calls
    printf("Rapid signal() sequence start\n");

    // These signal() calls are NOT atomic and create race conditions
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        printf("Atomicity attempt 1 failed\n");
    }

    if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
        printf("Atomicity attempt 2 failed\n");
    }

    if (signal(SIGTERM, atomicity_attempting_handler) == SIG_ERR) {
        printf("Atomicity attempt 3 failed\n");
    }

    if (signal(sig, atomicity_attempting_handler) == SIG_ERR) {
        printf("Atomicity attempt 4 failed\n");
    }

    printf("Rapid signal() sequence end (not actually atomic!)\n");

    // Additional misguided atomicity attempt
    if (atomicity_attempts % 3 == 0) {
        printf("Attempting 'atomic' reset of multiple signals\n");

        // These are also not atomic operations
        signal(SIGUSR1, SIG_DFL);
        signal(SIGUSR2, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        printf("'Atomic' reset complete (actually created race conditions)\n");
    }

    printf("Atomicity attempt complete (failed to achieve atomicity)\n");
}

int main() {
    printf("SIG34-C VIOLATION: Attempting signal() atomicity without proper protection\n");
    printf("Handler incorrectly assumes signal() calls can be made atomic\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, atomicity_attempting_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to see failed atomicity attempts\n");

    while (atomicity_attempts < 9) {
        pause();
    }

    printf("Atomicity attempts completed: %d (all failed to be truly atomic)\n", atomicity_attempts);
    return 0;
}