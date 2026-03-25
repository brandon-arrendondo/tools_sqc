/*
 * Rule: MSC38-C
 * Source: testcases
 * Status: PASS - Known limitation: pattern not detected
 * TODO: Move to fail/ when implemented (see PLAN.md)
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
