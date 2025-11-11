/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t last_signal = 0;

void atomic_handler(int sig) {
    // Compliant: Only using atomic operations and async-safe functions
    signal_count++;
    last_signal = sig;

    // write() is async-safe, unlike printf
    char msg[] = "Signal received\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main() {
    struct sigaction sa;

    sa.sa_handler = atomic_handler;
    sigemptyset(&sa.sa_mask);

    // Compliant: Mask the signal during its own handler execution
    sigaddset(&sa.sa_mask, SIGUSR1);

    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Handler uses only atomic operations - safe from interruption\n");

    while (1) {
        printf("Signals received: %d, last signal: %d\n", signal_count, last_signal);
        sleep(2);
    }

    return 0;
}