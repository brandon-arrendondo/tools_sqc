/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t persistence_attempts = 0;

void persistence_handler(int sig) {
    persistence_attempts++;
    printf("Signal %d received, attempting persistence via signal() (attempt %d)\n", sig, persistence_attempts);

    // VIOLATION: Trying to make signals persistent using signal()
    // This is the classic reason why signal() is problematic
    printf("Attempting to make handler persistent with signal()\n");

    if (signal(sig, persistence_handler) == SIG_ERR) {
        printf("Failed to re-establish handler for persistence\n");
        exit(EXIT_FAILURE);
    }

    printf("Handler re-registered for persistence\n");

    // Additional persistence attempts for other signals
    if (persistence_attempts % 3 == 0) {
        printf("Ensuring SIGTERM persistence\n");
        if (signal(SIGTERM, persistence_handler) == SIG_ERR) {
            printf("Failed to ensure SIGTERM persistence\n");
        }
    }

    // Race condition window exists between signal arrival and re-registration
    usleep(1500); // Simulate processing time during vulnerability window

    printf("Persistence establishment complete (but vulnerable)\n");
}

int main() {
    printf("SIG34-C VIOLATION: Attempting signal persistence with signal()\n");
    printf("Classic anti-pattern: using signal() to make handlers persistent\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, persistence_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 rapidly to expose persistence race conditions\n");

    while (persistence_attempts < 10) {
        pause();
    }

    printf("Persistence attempts completed: %d\n", persistence_attempts);
    return 0;
}