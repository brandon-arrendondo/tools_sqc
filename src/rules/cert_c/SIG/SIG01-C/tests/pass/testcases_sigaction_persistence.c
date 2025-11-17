/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void handler(int sig) {
    signal_count++;
    printf("Signal %d received (count: %d)\n", sig, signal_count);
}

int main() {
    struct sigaction sa;
    printf("Using sigaction() for reliable signal handler persistence\n");

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 multiple times - handler will persist reliably\n");

    while (signal_count < 5) {
        pause();
    }

    printf("Received %d signals total\n", signal_count);
    return 0;
}