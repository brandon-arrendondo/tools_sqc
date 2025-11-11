/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t sigusr1_compete = 0;
volatile sig_atomic_t sigusr2_compete = 0;
volatile sig_atomic_t sigterm_compete = 0;

void competing_handler(int sig) {
    printf("Competing handler called for signal %d\n", sig);

    if (sig == SIGUSR1) {
        sigusr1_compete++;
        printf("SIGUSR1 competing for signal() calls (count: %d)\n", sigusr1_compete);

        // VIOLATION: Multiple signals competing for signal() calls
        if (signal(SIGUSR2, competing_handler) == SIG_ERR) {
            printf("SIGUSR1 failed to register SIGUSR2\n");
        }
        if (signal(SIGTERM, competing_handler) == SIG_ERR) {
            printf("SIGUSR1 failed to register SIGTERM\n");
        }
    } else if (sig == SIGUSR2) {
        sigusr2_compete++;
        printf("SIGUSR2 competing for signal() calls (count: %d)\n", sigusr2_compete);

        // VIOLATION: Competing signal() calls from different handlers
        if (signal(SIGUSR1, competing_handler) == SIG_ERR) {
            printf("SIGUSR2 failed to register SIGUSR1\n");
        }
        if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
            printf("SIGUSR2 failed to ignore SIGTERM\n");
        }
    } else if (sig == SIGTERM) {
        sigterm_compete++;
        printf("SIGTERM competing for signal() calls (count: %d)\n", sigterm_compete);

        // VIOLATION: SIGTERM competing with other handlers
        if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) {
            printf("SIGTERM failed to reset SIGUSR1\n");
        }
        if (signal(SIGUSR2, SIG_DFL) == SIG_ERR) {
            printf("SIGTERM failed to reset SIGUSR2\n");
        }
    }

    printf("Competing signal() calls complete for signal %d\n", sig);
}

int main() {
    printf("SIG34-C VIOLATION: Multiple signals competing for signal() calls\n");
    printf("Different handlers compete to modify signal dispositions\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, competing_handler) == SIG_ERR) {
        perror("signal SIGUSR1");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1, SIGUSR2, and SIGTERM to see competition\n");

    while (sigusr1_compete + sigusr2_compete + sigterm_compete < 15) {
        pause();
    }

    printf("Signal competition completed\n");
    return 0;
}