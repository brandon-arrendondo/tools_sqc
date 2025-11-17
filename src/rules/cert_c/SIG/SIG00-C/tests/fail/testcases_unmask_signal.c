/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0;

void vulnerable_handler(int sig) {
    counter++;
    printf("Signal %d received, counter = %d\n", sig, counter);

    sleep(1);

    counter++;
    printf("Handler done, counter = %d\n", counter);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = vulnerable_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 to this process\n");

    while (1) {
        pause();
    }

    return 0;
}