/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t last_signal = 0;
volatile sig_atomic_t shutdown_flag = 0;

void safe_handler(int sig) {
    signal_count++;
    last_signal = sig;

    if (sig == SIGTERM) {
        shutdown_flag = 1;
    }
}

int main() {
    printf("Demonstrating safe signal handler using only sig_atomic_t\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, safe_handler);
    signal(SIGUSR2, safe_handler);
    signal(SIGTERM, safe_handler);

    printf("Send SIGUSR1, SIGUSR2, or SIGTERM\n");

    int previous_count = 0;
    while (!shutdown_flag) {
        if (signal_count != previous_count) {
            printf("Signal %d received (total: %d)\n",
                   last_signal, signal_count);
            previous_count = signal_count;
        }

        usleep(100000);
    }

    printf("Shutdown requested, exiting safely\n");
    return 0;
}