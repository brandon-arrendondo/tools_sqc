/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - printf and exit are not async-signal-safe
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/* printf in signal handler — not async-signal-safe */
void handler(int sig) {
    printf("caught signal %d\n", sig);
    exit(1);
}

void setup_handler(void) {
    signal(SIGINT, handler);
}
