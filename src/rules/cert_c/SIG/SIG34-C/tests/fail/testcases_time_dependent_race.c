/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t time_sensitive_count = 0;

void time_dependent_handler(int sig) {
    time_t current_time = time(NULL);
    time_sensitive_count++;

    printf("Signal %d at time %ld (count: %d)\n", sig, current_time, time_sensitive_count);

    // VIOLATION: Time-dependent signal() call creating race window
    if (current_time % 2 == 0) {
        printf("Even second: creating race window with signal() call\n");

        // Simulate delay that creates race condition window
        usleep(10000); // 10ms delay

        if (signal(sig, time_dependent_handler) == SIG_ERR) {
            printf("Failed to re-register during even second\n");
        } else {
            printf("Re-registered during race window\n");
        }
    } else {
        printf("Odd second: also creating race window\n");

        // Different timing but still vulnerable
        usleep(5000); // 5ms delay

        if (signal(sig, SIG_DFL) == SIG_ERR) {
            printf("Failed to set default during odd second\n");
        } else {
            printf("Set to default during race window\n");
        }
    }

    printf("Time-dependent signal() operation complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Time-dependent signal() calls in handler\n");
    printf("Creates race windows based on timing conditions\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, time_dependent_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 rapidly to expose time-dependent race conditions\n");

    while (time_sensitive_count < 10) {
        pause();
    }

    printf("Time-sensitive operations completed: %d\n", time_sensitive_count);
    return 0;
}