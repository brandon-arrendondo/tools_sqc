/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t counter1 = 0;
volatile sig_atomic_t counter2 = 0;

void handler1(int sig) {
    counter1++;
    printf("Handler1: Signal %d, count=%d\n", sig, counter1);
    sleep(2);
    printf("Handler1 complete\n");
}

void handler2(int sig) {
    counter2++;
    printf("Handler2: Signal %d, count=%d\n", sig, counter2);
    sleep(1);
    printf("Handler2 complete\n");
}

int main() {
    signal(SIGUSR1, handler1);
    signal(SIGUSR2, handler2);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 and SIGUSR2 rapidly\n");

    while (counter1 + counter2 < 10) {
        pause();
    }

    return 0;
}