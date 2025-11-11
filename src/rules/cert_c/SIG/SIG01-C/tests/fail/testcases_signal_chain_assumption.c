/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t step = 0;

void step_handler(int sig) {
    step++;
    printf("Step %d completed\n", step);

    if (step == 1) {
        printf("Triggering next step...\n");
        raise(SIGUSR1);  /* Assumes handler will still be there */
    }
}

int main() {
    printf("FAIL: Signal chain assuming handler persistence\n");

    signal(SIGUSR1, step_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 to start signal chain\n");

    /* Trigger the chain */
    raise(SIGUSR1);

    /* Wait for chain to complete */
    sleep(1);

    printf("Chain completed at step: %d\n", step);
    printf("Expected 2 steps, but handler may reset after first\n");

    return 0;
}