/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t masked_signals = 0;

void masked_handler(int sig) {
    masked_signals++;
    printf("Masked signal %d handled\n", masked_signals);
}

int main() {
    printf("FAIL: Signal masking with handler persistence assumption\n");

    signal(SIGUSR2, masked_handler);

    printf("PID: %d\n", getpid());

    /* Block SIGUSR2 temporarily */
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    printf("SIGUSR2 blocked, sending signals...\n");

    /* Send signals while masked */
    int i;
    for (i = 0; i < 3; i++) {
        raise(SIGUSR2);
        printf("Sent signal %d (blocked)\n", i + 1);
    }

    printf("Unblocking SIGUSR2, assuming handler still active\n");

    /* Unblock and assume handler is still there */
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    sleep(1);  /* Allow signals to be delivered */

    printf("Masked signals handled: %d\n", masked_signals);
    printf("Handler may have been reset during masking\n");

    return 0;
}