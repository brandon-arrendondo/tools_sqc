/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t flag = 0;

void bad_handler(int sig) {
    printf("Handler called with signal %d\n", sig);

    if (flag) {
        printf("Race condition detected!\n");
        exit(1);
    }

    flag = 1;

    sleep(2);

    flag = 0;
    printf("Handler complete\n");
}

int main() {
    signal(SIGINT, bad_handler);
    signal(SIGUSR1, bad_handler);

    printf("PID: %d\n", getpid());
    printf("Press Ctrl+C or send SIGUSR1 repeatedly to trigger race condition\n");

    while (1) {
        pause();
    }

    return 0;
}