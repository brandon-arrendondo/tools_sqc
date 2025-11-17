/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t registrations = 0;

void expanding_handler(int sig) {
    registrations++;
    printf("Handler called for signal %d (registrations: %d)\n", sig, registrations);

    // VIOLATION: Registering new signals from within handler
    switch (registrations) {
        case 1:
            printf("Registering SIGTERM handler\n");
            if (signal(SIGTERM, expanding_handler) == SIG_ERR) {
                printf("Failed to register SIGTERM\n");
            }
            break;
        case 2:
            printf("Registering SIGQUIT handler\n");
            if (signal(SIGQUIT, expanding_handler) == SIG_ERR) {
                printf("Failed to register SIGQUIT\n");
            }
            break;
        case 3:
            printf("Registering SIGPIPE handler\n");
            if (signal(SIGPIPE, expanding_handler) == SIG_ERR) {
                printf("Failed to register SIGPIPE\n");
            }
            break;
        case 4:
            printf("Registering SIGCHLD handler\n");
            if (signal(SIGCHLD, expanding_handler) == SIG_ERR) {
                printf("Failed to register SIGCHLD\n");
            }
            break;
        default:
            printf("Maximum registrations reached\n");
            break;
    }

    printf("New signal registration attempt complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Registering new signals from handler\n");
    printf("Handler progressively registers more signals using signal()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, expanding_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger progressive signal registration\n");

    while (registrations < 8) {
        pause();
    }

    printf("Total registrations performed: %d\n", registrations);
    return 0;
}