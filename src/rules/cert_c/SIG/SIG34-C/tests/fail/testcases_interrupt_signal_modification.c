/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t interrupt_count = 0;

void interrupt_modification_handler(int sig) {
    interrupt_count++;
    printf("Interrupt handler for signal %d (count: %d)\n", sig, interrupt_count);

    // VIOLATION: Modifying signal handlers during interrupt processing
    printf("Modifying signal dispositions during interrupt processing\n");

    // Dangerous: modifying signal handling during critical interrupt processing
    if (interrupt_count % 2 == 1) {
        printf("Odd interrupt: disabling critical signals\n");
        if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
            printf("Failed to ignore SIGTERM during interrupt\n");
        }
        if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
            printf("Failed to ignore SIGINT during interrupt\n");
        }
    } else {
        printf("Even interrupt: re-enabling critical signals\n");
        if (signal(SIGTERM, interrupt_modification_handler) == SIG_ERR) {
            printf("Failed to handle SIGTERM during interrupt\n");
        }
        if (signal(SIGINT, interrupt_modification_handler) == SIG_ERR) {
            printf("Failed to handle SIGINT during interrupt\n");
        }
    }

    // Always try to maintain this handler during interrupts
    if (signal(sig, interrupt_modification_handler) == SIG_ERR) {
        printf("Failed to maintain handler during interrupt\n");
    }

    // Simulate interrupt processing time where signals could arrive
    usleep(500);

    printf("Interrupt signal modification complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Modifying signals during interrupt processing\n");
    printf("Handler modifies signal dispositions during critical interrupt handling\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, interrupt_modification_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger interrupt signal modifications\n");

    while (interrupt_count < 8) {
        pause();
    }

    printf("Interrupt modifications completed: %d\n", interrupt_count);
    return 0;
}