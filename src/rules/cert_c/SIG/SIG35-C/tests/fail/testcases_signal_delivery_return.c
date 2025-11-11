/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_cascade = 0;

void signal_delivery_handler(int sig) {
    printf("Exception handler: Attempting signal delivery\n");
    signal_cascade++;

    /* Attempt to send signals to self */
    printf("Sending SIGUSR1 to self\n");
    if (kill(getpid(), SIGUSR1) == 0) {
        printf("SIGUSR1 sent successfully\n");
    } else {
        printf("Failed to send SIGUSR1\n");
    }

    /* Attempt to send signal to parent process */
    printf("Sending SIGUSR2 to parent\n");
    if (kill(getppid(), SIGUSR2) == 0) {
        printf("SIGUSR2 sent to parent\n");
    } else {
        printf("Failed to send SIGUSR2 to parent\n");
    }

    printf("Signal delivery attempts complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

void user_signal_handler(int sig) {
    printf("User signal %d received (cascade level: %d)\n", sig, signal_cascade);
}

int main() {
    printf("Testing signal delivery in exception handler with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, signal_delivery_handler);
    signal(SIGUSR1, user_signal_handler);
    signal(SIGUSR2, user_signal_handler);

    printf("Signal cascade level: %d\n", signal_cascade);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Signal cascade after exception: %d\n", signal_cascade);
    printf("This represents undefined behavior\n");

    return 0;
}