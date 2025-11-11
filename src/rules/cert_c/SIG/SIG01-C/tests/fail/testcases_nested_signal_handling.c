/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t nested_count = 0;
volatile sig_atomic_t interrupt_count = 0;

void nested_handler(int sig) {
    nested_count++;
    printf("Nested handler called: %d\n", nested_count);

    if (sig == SIGUSR1) {
        /* Assumes SIGINT handler is still active */
        printf("Triggering interrupt from nested handler\n");
        raise(SIGINT);
    }
}

void interrupt_handler(int sig) {
    interrupt_count++;
    printf("Interrupt handler called: %d\n", interrupt_count);
}

int main() {
    printf("FAIL: Nested signal handling with persistence assumption\n");

    signal(SIGUSR1, nested_handler);
    signal(SIGINT, interrupt_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 to trigger nested signal handling\n");

    raise(SIGUSR1);

    sleep(1);  /* Allow nested signals to process */

    printf("Nested calls: %d, Interrupts: %d\n", nested_count, interrupt_count);
    printf("Assumes both handlers remain active during nesting\n");

    return 0;
}