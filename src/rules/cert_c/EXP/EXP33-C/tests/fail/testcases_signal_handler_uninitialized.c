/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: signal_handler_uninitialized.c
 */

#include <stdio.h>
#include <signal.h>

static int signal_count;  /* Uninitialized global */

/* NON-COMPLIANT: Signal handler uses uninitialized data */
void signal_handler(int sig) {
    signal_count++;  /* Increments uninitialized value */
    printf("Signal received %d times\n", signal_count);
}

int main(void) {
    signal(SIGINT, signal_handler);
    raise(SIGINT);  /* Trigger signal handler */
    return 0;
}