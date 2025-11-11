/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t async_count = 0;
volatile sig_atomic_t processing = 0;

void async_handler(int sig) {
    async_count++;
    processing = 1;
    printf("Async signal %d processing\n", async_count);

    /* Simulate async work */
    sleep(1);

    processing = 0;
    printf("Async signal %d complete\n", async_count);
}

int main() {
    printf("FAIL: Asynchronous signal handling persistence assumption\n");

    signal(SIGALRM, async_handler);

    printf("PID: %d\n", getpid());
    printf("Testing asynchronous signal delivery\n");

    /* Set multiple alarms assuming handler persists */
    alarm(1);
    alarm(2);  /* This overwrites the first alarm */

    printf("Alarms set, waiting for async signals\n");

    /* Wait for signals assuming handler will process them */
    while (async_count < 2) {
        if (!processing) {
            usleep(100000);
        }
    }

    printf("Async signals processed: %d\n", async_count);
    printf("Code assumes handler remains active for async signals\n");

    return 0;
}