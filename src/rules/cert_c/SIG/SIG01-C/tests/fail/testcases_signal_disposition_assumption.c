/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t disposition_count = 0;

void disposition_handler(int sig) {
    disposition_count++;
    printf("Signal disposition handler called: %d\n", disposition_count);
}

int main() {
    printf("FAIL: Signal disposition assumption across exec\n");

    signal(SIGUSR1, disposition_handler);

    printf("PID: %d\n", getpid());
    printf("Setting signal disposition, assuming it persists\n");

    /* Test signal handler */
    raise(SIGUSR1);
    sleep(1);

    printf("Before exec, signals handled: %d\n", disposition_count);

    /* Assume signal disposition persists across exec */
    printf("Code assumes signal handler survives across exec calls\n");
    printf("This is incorrect - signal handlers are reset on exec\n");

    /* Simulate what would happen if we exec'd */
    printf("If this were followed by exec(), handler would be lost\n");
    printf("Code incorrectly assumes signal(SIGUSR1, handler) persists\n");

    return 0;
}