/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t processing = 0;

void process_handler(int sig) {
    processing = 1;
    printf("Processing signal...\n");
    sleep(2);  /* Simulate work */
    processing = 0;
    printf("Processing complete\n");
}

int main() {
    printf("FAIL: Ignoring implicit handler reset during signal processing\n");

    signal(SIGTERM, process_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGTERM multiple times during processing\n");

    int signals_sent = 0;
    while (signals_sent < 3) {
        raise(SIGTERM);
        signals_sent++;
        printf("Sent signal %d\n", signals_sent);

        /* Assumes handler is still active for next signal */
        usleep(500000);  /* 0.5 seconds */
    }

    /* Wait for any remaining processing */
    while (processing) {
        usleep(100000);
    }

    printf("All signals sent, handler may have been reset\n");
    return 0;
}