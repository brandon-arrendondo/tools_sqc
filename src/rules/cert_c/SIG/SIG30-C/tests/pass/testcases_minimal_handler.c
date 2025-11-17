/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t shutdown_requested = 0;

void shutdown_handler(int sig) {
    shutdown_requested = 1;
}

void cleanup_handler(int sig) {
    _Exit(EXIT_SUCCESS);
}

int main() {
    printf("Demonstrating minimal safe signal handlers\n");
    printf("PID: %d\n", getpid());

    signal(SIGTERM, shutdown_handler);
    signal(SIGINT, shutdown_handler);
    signal(SIGUSR1, cleanup_handler);

    printf("Send SIGTERM/SIGINT for graceful shutdown, SIGUSR1 for immediate exit\n");

    int counter = 0;
    while (!shutdown_requested) {
        printf("Working... %d\n", ++counter);
        sleep(1);

        if (counter >= 10) {
            printf("Work completed normally\n");
            break;
        }
    }

    if (shutdown_requested) {
        printf("Shutdown requested, cleaning up...\n");
        sleep(1);
        printf("Cleanup complete\n");
    }

    return 0;
}