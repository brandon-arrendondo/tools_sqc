/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// SAFE: Using volatile sig_atomic_t for signal communication
volatile sig_atomic_t sigusr1_count = 0;
volatile sig_atomic_t sigusr2_count = 0;
volatile sig_atomic_t total_signals = 0;
volatile sig_atomic_t exit_requested = 0;

void counting_handler(int sig) {
    // SAFE: Only atomic operations on sig_atomic_t variables
    switch (sig) {
        case SIGUSR1:
            sigusr1_count++;
            break;
        case SIGUSR2:
            sigusr2_count++;
            break;
        case SIGTERM:
            exit_requested = 1;
            break;
    }

    // SAFE: Atomic increment
    total_signals++;
}

int main() {
    printf("Demonstrating safe signal counting\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, counting_handler);
    signal(SIGUSR2, counting_handler);
    signal(SIGTERM, counting_handler);

    printf("Send SIGUSR1, SIGUSR2 signals to increment counters\n");
    printf("Send SIGTERM to exit gracefully\n");
    printf("Signal handler only modifies sig_atomic_t variables - safe!\n");

    while (!exit_requested) {
        // Display current counts (safe to do in main context)
        printf("SIGUSR1: %d, SIGUSR2: %d, Total: %d\n",
               (int)sigusr1_count, (int)sigusr2_count, (int)total_signals);

        sleep(2);
    }

    printf("\nExiting due to SIGTERM...\n");
    printf("Final counts - SIGUSR1: %d, SIGUSR2: %d, Total: %d\n",
           (int)sigusr1_count, (int)sigusr2_count, (int)total_signals);

    return 0;
}