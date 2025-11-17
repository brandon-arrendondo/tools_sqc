/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t registration_count = 0;

void verified_handler(int sig) {
    registration_count++;
    printf("Verified handler called: %d\n", registration_count);
}

int verify_signal_registration(int sig, void (*handler)(int)) {
    struct sigaction sa, old_sa;

    /* Get current signal disposition */
    if (sigaction(sig, NULL, &old_sa) == -1) {
        perror("sigaction get");
        return 0;
    }

    /* Check if our handler is actually registered */
    return (old_sa.sa_handler == handler);
}

int main() {
    struct sigaction sa;
    printf("PASS: Proper signal handler registration with verification\n");

    /* Set up sigaction structure */
    sa.sa_handler = verified_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /* Register handler */
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());

    /* Verify registration was successful */
    if (verify_signal_registration(SIGUSR2, verified_handler)) {
        printf("Signal handler registration verified successfully\n");
    } else {
        printf("Signal handler registration failed verification\n");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR2 to test verified handler\n");

    /* Test the verified handler */
    raise(SIGUSR2);
    sleep(1);

    /* Re-verify after signal delivery */
    if (verify_signal_registration(SIGUSR2, verified_handler)) {
        printf("Handler remains registered after signal delivery\n");
    } else {
        printf("WARNING: Handler no longer registered after delivery\n");
    }

    printf("Registration count: %d\n", registration_count);
    return 0;
}