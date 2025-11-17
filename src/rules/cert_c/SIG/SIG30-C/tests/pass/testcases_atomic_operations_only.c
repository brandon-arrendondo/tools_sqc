/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// SAFE: All signal handler variables are volatile sig_atomic_t
volatile sig_atomic_t signal_mask = 0;     // Bitmask of received signals
volatile sig_atomic_t signal_counter = 0;  // Total signal count
volatile sig_atomic_t last_signal = 0;     // Most recent signal
volatile sig_atomic_t exit_flag = 0;       // Exit request flag

// Signal bit positions
#define SIGUSR1_BIT (1 << 0)
#define SIGUSR2_BIT (1 << 1)
#define SIGTERM_BIT (1 << 2)

void atomic_only_handler(int sig) {
    // SAFE: All operations are on sig_atomic_t variables
    // These are guaranteed to be atomic

    // Set bit in signal mask
    switch (sig) {
        case SIGUSR1:
            signal_mask |= SIGUSR1_BIT;
            break;
        case SIGUSR2:
            signal_mask |= SIGUSR2_BIT;
            break;
        case SIGTERM:
            signal_mask |= SIGTERM_BIT;
            exit_flag = 1;
            break;
    }

    // Update counters atomically
    signal_counter++;
    last_signal = sig;
}

int main() {
    printf("Demonstrating signal handler with only atomic operations\n");
    printf("PID: %d\n", getpid());

    // Install signal handlers
    signal(SIGUSR1, atomic_only_handler);
    signal(SIGUSR2, atomic_only_handler);
    signal(SIGTERM, atomic_only_handler);

    printf("Signal handler uses only sig_atomic_t variables\n");
    printf("All operations are guaranteed atomic - completely safe!\n");
    printf("Send SIGUSR1, SIGUSR2, or SIGTERM\n");

    while (!exit_flag) {
        // Read signal state atomically
        sig_atomic_t current_mask = signal_mask;
        sig_atomic_t current_count = signal_counter;
        sig_atomic_t current_last = last_signal;

        // Process signals based on mask
        if (current_mask & SIGUSR1_BIT) {
            printf("SIGUSR1 detected in mask\n");
            signal_mask &= ~SIGUSR1_BIT;  // Clear bit
        }

        if (current_mask & SIGUSR2_BIT) {
            printf("SIGUSR2 detected in mask\n");
            signal_mask &= ~SIGUSR2_BIT;  // Clear bit
        }

        if (current_mask & SIGTERM_BIT) {
            printf("SIGTERM detected in mask\n");
            signal_mask &= ~SIGTERM_BIT;  // Clear bit
        }

        // Display statistics
        printf("Total signals: %d, Last signal: %d\n",
               (int)current_count, (int)current_last);

        sleep(1);
    }

    printf("Exiting due to SIGTERM...\n");
    printf("Final count: %d signals processed\n", (int)signal_counter);

    return 0;
}