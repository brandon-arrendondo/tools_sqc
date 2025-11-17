/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0;

void safe_handler(int sig) {
    counter++;
    printf("Signal %d received, counter = %d\n", sig, counter);

    // Safe: Handler cannot be interrupted by other signals
    // due to proper signal masking in sigaction
    sleep(2);

    counter++;
    printf("Handler done, counter = %d\n", counter);
}

int main() {
    struct sigaction sa;

    sa.sa_handler = safe_handler;
    sigemptyset(&sa.sa_mask);

    // Compliant: Mask all signals that could interfere during handler execution
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGINT);

    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 and SIGUSR2 - handlers cannot interrupt each other\n");

    while (1) {
        pause();
    }

    return 0;
}