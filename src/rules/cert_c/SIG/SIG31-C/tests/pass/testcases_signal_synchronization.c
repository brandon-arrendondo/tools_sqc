/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Compliant: Signal synchronization without shared object access */
volatile sig_atomic_t sync_signal_received = 0;
volatile sig_atomic_t sync_signal_type = 0;

void sync_signal_handler(int sig) {
    /* Compliant: Only setting atomic flags for synchronization */
    sync_signal_received = 1;
    sync_signal_type = sig;
}

int wait_for_signal(int expected_signal, int timeout_seconds) {
    /* Safe synchronization function that waits for specific signal */
    int elapsed = 0;

    sync_signal_received = 0;
    sync_signal_type = 0;

    while (elapsed < timeout_seconds) {
        if (sync_signal_received) {
            if (sync_signal_type == expected_signal) {
                sync_signal_received = 0;  /* Reset for next use */
                return 1;  /* Success */
            } else {
                printf("Unexpected signal %d (expected %d)\n",
                       (int)sync_signal_type, expected_signal);
                sync_signal_received = 0;  /* Reset and continue waiting */
            }
        }

        sleep(1);
        elapsed++;
    }

    return 0;  /* Timeout */
}

int main() {
    printf("Demonstrating safe signal synchronization without shared access\n");
    printf("PID: %d\n", getpid());

    /* Install signal handler */
    signal(SIGUSR1, sync_signal_handler);
    signal(SIGUSR2, sync_signal_handler);
    signal(SIGTERM, sync_signal_handler);

    /* Main program state (not accessed by signal handler) */
    int phase = 1;
    int operations_completed = 0;
    char phase_description[256];

    printf("Signal synchronization demo:\n");
    printf("  Send SIGUSR1 to advance to next phase\n");
    printf("  Send SIGUSR2 to trigger operation\n");
    printf("  Send SIGTERM to exit\n");

    while (1) {
        switch (phase) {
            case 1:
                strcpy(phase_description, "Initialization phase");
                printf("\nPhase 1: %s\n", phase_description);
                printf("Waiting for SIGUSR1 to continue...\n");

                if (wait_for_signal(SIGUSR1, 10)) {
                    printf("Phase 1 completed, advancing to phase 2\n");
                    phase = 2;
                } else {
                    printf("Timeout in phase 1, retrying...\n");
                }
                break;

            case 2:
                strcpy(phase_description, "Processing phase");
                printf("\nPhase 2: %s\n", phase_description);
                printf("Send SIGUSR2 to perform operation, SIGUSR1 to advance\n");

                while (phase == 2) {
                    /* Wait for any signal with 5 second timeout */
                    sync_signal_received = 0;
                    int timeout = 0;

                    while (!sync_signal_received && timeout < 5) {
                        sleep(1);
                        timeout++;
                    }

                    if (sync_signal_received) {
                        if (sync_signal_type == SIGUSR2) {
                            operations_completed++;
                            printf("Operation %d completed\n", operations_completed);
                            sync_signal_received = 0;
                        } else if (sync_signal_type == SIGUSR1) {
                            printf("Phase 2 completed with %d operations\n",
                                   operations_completed);
                            phase = 3;
                            sync_signal_received = 0;
                        } else if (sync_signal_type == SIGTERM) {
                            printf("Termination requested in phase 2\n");
                            goto cleanup;
                        }
                    } else {
                        printf("Phase 2: waiting for signals... (ops completed: %d)\n",
                               operations_completed);
                    }
                }
                break;

            case 3:
                strcpy(phase_description, "Cleanup phase");
                printf("\nPhase 3: %s\n", phase_description);
                printf("Send SIGTERM to exit cleanly\n");

                if (wait_for_signal(SIGTERM, 15)) {
                    printf("Clean shutdown requested\n");
                    goto cleanup;
                } else {
                    printf("Timeout waiting for SIGTERM, restarting phases\n");
                    phase = 1;
                    operations_completed = 0;
                }
                break;

            default:
                printf("Unknown phase %d, resetting\n", phase);
                phase = 1;
                break;
        }
    }

cleanup:
    printf("Program completed safely with signal synchronization\n");
    printf("Total operations completed: %d\n", operations_completed);
    return 0;
}