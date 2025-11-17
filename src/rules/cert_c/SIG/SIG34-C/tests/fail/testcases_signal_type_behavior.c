/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t type_based_calls = 0;

void type_dependent_handler(int sig) {
    type_based_calls++;
    printf("Handler called for signal %d, modifying behavior based on type (call %d)\n", sig, type_based_calls);

    // VIOLATION: Modifying signal behavior based on signal type using signal()
    switch (sig) {
        case SIGUSR1:
            printf("SIGUSR1: Setting up termination signals\n");
            if (signal(SIGTERM, type_dependent_handler) == SIG_ERR) {
                printf("Failed to handle SIGTERM\n");
            }
            if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
                printf("Failed to ignore SIGINT\n");
            }
            break;

        case SIGUSR2:
            printf("SIGUSR2: Setting up I/O signals\n");
            if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
                printf("Failed to ignore SIGPIPE\n");
            }
            if (signal(SIGIO, type_dependent_handler) == SIG_ERR) {
                printf("Failed to handle SIGIO\n");
            }
            break;

        case SIGTERM:
            printf("SIGTERM: Resetting user signals\n");
            if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) {
                printf("Failed to reset SIGUSR1\n");
            }
            if (signal(SIGUSR2, SIG_DFL) == SIG_ERR) {
                printf("Failed to reset SIGUSR2\n");
            }
            break;

        case SIGINT:
            printf("SIGINT: Emergency signal() calls\n");
            if (signal(SIGQUIT, type_dependent_handler) == SIG_ERR) {
                printf("Failed to handle SIGQUIT\n");
            }
            break;

        default:
            printf("Unknown signal: default signal() behavior\n");
            if (signal(sig, type_dependent_handler) == SIG_ERR) {
                printf("Failed to re-register unknown signal\n");
            }
            break;
    }

    printf("Type-dependent signal() modification complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Modifying signal behavior based on signal type\n");
    printf("Handler uses signal() differently based on which signal triggered it\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, type_dependent_handler) == SIG_ERR) {
        perror("signal SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR2, type_dependent_handler) == SIG_ERR) {
        perror("signal SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1, SIGUSR2, SIGTERM, SIGINT to see type-dependent behavior\n");

    while (type_based_calls < 10) {
        pause();
    }

    printf("Type-dependent calls completed: %d\n", type_based_calls);
    return 0;
}