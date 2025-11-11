/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t swap_count = 0;

void primary_handler(int sig);
void secondary_handler(int sig);

void primary_handler(int sig) {
    swap_count++;
    printf("PRIMARY handler for signal %d (swap %d)\n", sig, swap_count);

    // VIOLATION: Swapping to secondary handler using signal()
    printf("Swapping to secondary handler\n");
    if (signal(sig, secondary_handler) == SIG_ERR) {
        printf("Failed to swap to secondary handler\n");
    } else {
        printf("Successfully swapped to secondary handler\n");
    }

    // Also set up secondary for other signals
    if (signal(SIGUSR2, secondary_handler) == SIG_ERR) {
        printf("Failed to set secondary for SIGUSR2\n");
    }
}

void secondary_handler(int sig) {
    swap_count++;
    printf("SECONDARY handler for signal %d (swap %d)\n", sig, swap_count);

    // VIOLATION: Swapping back to primary handler using signal()
    printf("Swapping back to primary handler\n");
    if (signal(sig, primary_handler) == SIG_ERR) {
        printf("Failed to swap back to primary handler\n");
    } else {
        printf("Successfully swapped back to primary handler\n");
    }

    // Set up primary for other signals
    if (signal(SIGTERM, primary_handler) == SIG_ERR) {
        printf("Failed to set primary for SIGTERM\n");
    }
}

int main() {
    printf("SIG34-C VIOLATION: Handler swapping using signal() calls\n");
    printf("Handlers continuously swap between primary and secondary using signal()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, primary_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1, SIGUSR2, SIGTERM to see handler swapping\n");

    while (swap_count < 10) {
        pause();
    }

    printf("Handler swaps completed: %d\n", swap_count);
    return 0;
}