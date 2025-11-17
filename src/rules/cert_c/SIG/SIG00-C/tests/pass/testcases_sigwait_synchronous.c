/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main() {
    sigset_t signal_set;
    int received_signal;

    printf("PID: %d\n", getpid());
    printf("Using sigwait for synchronous signal handling\n");

    // Compliant: Block signals for synchronous handling
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGUSR1);
    sigaddset(&signal_set, SIGUSR2);
    sigaddset(&signal_set, SIGTERM);

    // Block these signals for all threads
    if (sigprocmask(SIG_BLOCK, &signal_set, NULL) != 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("Signals blocked, waiting synchronously\n");

    while (1) {
        // Wait for signal synchronously - no signal handler needed
        if (sigwait(&signal_set, &received_signal) == 0) {
            printf("Received signal %d via sigwait\n", received_signal);

            // Safe: Running in normal thread context, not signal handler
            switch (received_signal) {
                case SIGUSR1:
                    printf("Processing SIGUSR1 safely\n");
                    // Complex operations are safe here
                    for (int i = 0; i < 3; i++) {
                        printf("  SIGUSR1 processing step %d\n", i + 1);
                        sleep(1);
                    }
                    break;

                case SIGUSR2:
                    printf("Processing SIGUSR2 safely\n");
                    // File operations are safe here
                    FILE* fp = fopen("/tmp/sigusr2_log.txt", "a");
                    if (fp) {
                        fprintf(fp, "SIGUSR2 received at time %ld\n", time(NULL));
                        fclose(fp);
                    }
                    break;

                case SIGTERM:
                    printf("Received SIGTERM, performing clean shutdown\n");
                    // Safe cleanup operations
                    printf("Cleanup step 1: Saving state\n");
                    sleep(1);
                    printf("Cleanup step 2: Releasing resources\n");
                    sleep(1);
                    printf("Cleanup complete, exiting\n");
                    exit(0);

                default:
                    printf("Unexpected signal %d\n", received_signal);
                    break;
            }

            printf("Signal %d processing complete\n\n", received_signal);
        } else {
            perror("sigwait");
            break;
        }
    }

    return 0;
}