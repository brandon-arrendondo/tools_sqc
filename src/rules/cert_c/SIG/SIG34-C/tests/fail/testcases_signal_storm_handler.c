/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t storm_count = 0;

void signal_storm_handler(int sig) {
    storm_count++;
    printf("Signal storm handler for signal %d (count: %d)\n", sig, storm_count);

    // VIOLATION: Creating signal storm with multiple signal() calls
    printf("Creating signal registration storm\n");

    // Register multiple handlers rapidly, creating race conditions
    if (signal(SIGPIPE, signal_storm_handler) == SIG_ERR) {
        printf("Storm registration 1 failed\n");
    }

    if (signal(SIGCHLD, signal_storm_handler) == SIG_ERR) {
        printf("Storm registration 2 failed\n");
    }

    if (signal(SIGTERM, signal_storm_handler) == SIG_ERR) {
        printf("Storm registration 3 failed\n");
    }

    if (signal(SIGQUIT, signal_storm_handler) == SIG_ERR) {
        printf("Storm registration 4 failed\n");
    }

    if (signal(SIGINT, signal_storm_handler) == SIG_ERR) {
        printf("Storm registration 5 failed\n");
    }

    // Re-register self to maintain storm
    if (signal(sig, signal_storm_handler) == SIG_ERR) {
        printf("Self storm registration failed\n");
    }

    printf("Signal registration storm complete (created %d race conditions)\n", 6);
}

int main() {
    printf("SIG34-C VIOLATION: Signal registration storm using signal() in handler\n");
    printf("Handler creates multiple rapid signal() calls causing race conditions\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, signal_storm_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger signal registration storm\n");

    while (storm_count < 5) {
        pause();
    }

    printf("Signal storms completed: %d\n", storm_count);
    return 0;
}