/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Async-signal-unsafe functions in signal handlers
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/* printf in signal handler */
void handler_printf(int sig) {
    printf("signal %d\n", sig);
}

/* malloc in signal handler */
void handler_malloc(int sig) {
    void *p = malloc(100);
    free(p);
    (void)sig;
}

void setup_handlers(void) {
    signal(SIGINT, handler_printf);
    signal(SIGTERM, handler_malloc);
}
