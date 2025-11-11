/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void safe_handler(int sig) {
    signal_count++;
    printf("Signal %d received (count: %d)\n", sig, signal_count);
    printf("Handler remains persistent automatically with sigaction\n");
}

int main() {
    struct sigaction sa;
    printf("Using sigaction() for persistent signal handling (SAFE)\n");
    printf("PID: %d\n", getpid());

    sa.sa_handler = safe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Send multiple SIGUSR1 signals - no race condition possible\n");

    while (signal_count < 10) {
        pause();
    }

    printf("Received %d signals safely\n", signal_count);
    return 0;
}