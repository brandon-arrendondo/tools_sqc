/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_received = 0;
volatile sig_atomic_t signal_type = 0;

void safe_handler(int sig) {
    signal_received = 1;
    signal_type = sig;
}

int main() {
    printf("Demonstrating safe signal handler (only sets flags)\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, safe_handler);
    signal(SIGUSR2, safe_handler);

    printf("Send SIGUSR1 or SIGUSR2 to trigger safe handler\n");

    while (1) {
        if (signal_received) {
            printf("Signal %d was received safely\n", signal_type);

            if (signal_type == SIGUSR1) {
                printf("Processing SIGUSR1...\n");
            } else if (signal_type == SIGUSR2) {
                printf("Processing SIGUSR2...\n");
                break;
            }

            signal_received = 0;
            signal_type = 0;
        }

        usleep(100000);
    }

    printf("Program terminated safely\n");
    return 0;
}