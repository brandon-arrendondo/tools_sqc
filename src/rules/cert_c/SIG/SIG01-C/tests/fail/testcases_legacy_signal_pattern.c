/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t interrupt_count = 0;

void interrupt_handler(int sig) {
    interrupt_count++;
    printf("Interrupt %d received\n", interrupt_count);

    /* Legacy pattern: re-register handler without checking if needed */
    signal(SIGINT, interrupt_handler);  /* May be redundant or harmful */
}

int main() {
    printf("FAIL: Legacy signal handling pattern with assumptions\n");

    /* Old-style signal registration */
    if (signal(SIGINT, interrupt_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d - Press Ctrl+C multiple times\n", getpid());
    printf("Using legacy signal() with re-registration in handler\n");

    while (interrupt_count < 5) {
        pause();
    }

    printf("Total interrupts: %d\n", interrupt_count);
    return 0;
}