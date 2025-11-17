/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t critical_section = 0;

void timer_handler(int sig) {
    critical_section = 1;
    printf("Timer expired - entering critical section\n");
    /* Handler may be reset here - race condition */
}

int main() {
    printf("FAIL: Race condition due to handler reset assumption\n");

    signal(SIGALRM, timer_handler);

    printf("PID: %d\n", getpid());
    printf("Setting multiple alarms - assuming handler persists\n");

    /* Set multiple alarms assuming handler will persist */
    alarm(1);
    alarm(2);
    alarm(3);

    while (critical_section < 1) {
        pause();
    }

    printf("Critical section flag: %d\n", critical_section);
    printf("Code vulnerable to handler reset between signals\n");

    return 0;
}