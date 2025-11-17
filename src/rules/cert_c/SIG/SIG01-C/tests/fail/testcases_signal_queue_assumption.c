/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t queued_signals = 0;

void queue_handler(int sig) {
    queued_signals++;
    printf("Queued signal processed: %d\n", queued_signals);
    sleep(1);  /* Simulate slow processing */
}

int main() {
    printf("FAIL: Signal queueing with handler persistence assumption\n");

    signal(SIGUSR1, queue_handler);

    printf("PID: %d\n", getpid());
    printf("Sending rapid signals assuming queueing and persistence\n");

    /* Send signals rapidly, assuming they queue and handler persists */
    int i;
    for (i = 0; i < 8; i++) {
        raise(SIGUSR1);
        printf("Sent signal %d\n", i + 1);
        usleep(100000);  /* 0.1 seconds */
    }

    /* Wait for processing */
    sleep(10);

    printf("Signals processed: %d (sent 8)\n", queued_signals);
    printf("Assumes handler persists and signals queue properly\n");

    return 0;
}