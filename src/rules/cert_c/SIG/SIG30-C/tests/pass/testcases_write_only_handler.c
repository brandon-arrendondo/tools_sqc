/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Pre-computed messages to avoid any string operations in handler
static const char sigusr1_msg[] = "1\n";  // Just signal number
static const char sigusr2_msg[] = "2\n";
static const char sigterm_msg[] = "T\n";  // T for terminate

void write_only_handler(int sig) {
    // SAFE: Only use write() system call - most restrictive safe approach
    switch (sig) {
        case SIGUSR1:
            write(STDOUT_FILENO, sigusr1_msg, sizeof(sigusr1_msg) - 1);
            break;
        case SIGUSR2:
            write(STDOUT_FILENO, sigusr2_msg, sizeof(sigusr2_msg) - 1);
            break;
        case SIGTERM:
            write(STDOUT_FILENO, sigterm_msg, sizeof(sigterm_msg) - 1);
            break;
    }

    // No variables modified, no memory allocation, no complex operations
    // Just the absolute minimum: write() system call
}

int main() {
    printf("Demonstrating ultra-safe signal handler using only write()\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, write_only_handler);
    signal(SIGUSR2, write_only_handler);
    signal(SIGTERM, write_only_handler);

    printf("Signal handlers will output:\n");
    printf("- SIGUSR1: '1'\n");
    printf("- SIGUSR2: '2'\n");
    printf("- SIGTERM: 'T'\n");
    printf("This is the safest possible signal handler implementation.\n");

    printf("Send signals to see minimal safe output:\n");

    // Main loop that interprets the simple output
    char buffer[2];
    while (1) {
        printf("Waiting for signals... ");
        fflush(stdout);

        // Check if we can read any signal output
        // In a real application, you might use select() or similar
        sleep(2);
        printf("(send SIGUSR1, SIGUSR2, or SIGTERM)\n");
    }

    return 0;
}