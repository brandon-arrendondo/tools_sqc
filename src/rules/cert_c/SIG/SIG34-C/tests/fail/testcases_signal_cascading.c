/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t cascade_level = 0;

void cascading_signal_handler(int sig) {
    cascade_level++;
    printf("Cascading handler level %d for signal %d\n", cascade_level, sig);

    // VIOLATION: Cascading signal() calls that trigger more handlers
    if (cascade_level < 4) {
        printf("Level %d: Setting up cascade for next level\n", cascade_level);

        // Each level sets up the next signal in the cascade
        switch (cascade_level) {
            case 1:
                printf("Level 1: Triggering SIGUSR2 cascade\n");
                if (signal(SIGUSR2, cascading_signal_handler) == SIG_ERR) {
                    printf("Failed to set SIGUSR2 cascade\n");
                }
                raise(SIGUSR2); // Trigger next level
                break;

            case 2:
                printf("Level 2: Triggering SIGTERM cascade\n");
                if (signal(SIGTERM, cascading_signal_handler) == SIG_ERR) {
                    printf("Failed to set SIGTERM cascade\n");
                }
                raise(SIGTERM); // Trigger next level
                break;

            case 3:
                printf("Level 3: Triggering SIGQUIT cascade\n");
                if (signal(SIGQUIT, cascading_signal_handler) == SIG_ERR) {
                    printf("Failed to set SIGQUIT cascade\n");
                }
                raise(SIGQUIT); // Trigger next level
                break;
        }

        // Re-register self for potential re-entry
        if (signal(sig, cascading_signal_handler) == SIG_ERR) {
            printf("Failed to re-register at level %d\n", cascade_level);
        }
    } else {
        printf("Maximum cascade level reached, resetting\n");
        cascade_level = 0;

        // Reset all signal handlers
        signal(SIGUSR1, SIG_DFL);
        signal(SIGUSR2, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
    }

    printf("Cascade level %d processing complete\n", cascade_level);
}

int main() {
    printf("SIG34-C VIOLATION: Cascading signal() calls triggering handler chains\n");
    printf("Each handler level triggers the next using signal() and raise()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, cascading_signal_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger signal cascade\n");

    // Trigger the cascade
    raise(SIGUSR1);

    sleep(2); // Allow cascade to complete

    printf("Signal cascade demonstration complete\n");
    return 0;
}