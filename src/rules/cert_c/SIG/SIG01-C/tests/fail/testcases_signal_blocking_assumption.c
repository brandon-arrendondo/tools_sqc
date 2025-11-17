/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t blocked_count = 0;

void blocking_handler(int sig) {
    blocked_count++;
    printf("Signal %d handled while others blocked\n", blocked_count);
}

int main() {
    printf("FAIL: Signal blocking behavior assumption\n");

    signal(SIGUSR1, blocking_handler);

    printf("PID: %d\n", getpid());

    /* Block SIGUSR1 */
    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &block_set, NULL);

    printf("SIGUSR1 blocked\n");

    /* Send signals while blocked */
    raise(SIGUSR1);
    raise(SIGUSR1);
    raise(SIGUSR1);

    printf("Sent 3 signals while blocked\n");

    /* Unblock - assumes all signals will be delivered */
    printf("Unblocking, assuming all 3 signals will be delivered\n");
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    sleep(1);

    printf("Signals delivered: %d (expected 3, but may get only 1)\n", blocked_count);
    printf("Standard signals don't queue - only one may be delivered\n");

    return 0;
}