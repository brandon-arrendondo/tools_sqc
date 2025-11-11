/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void vulnerable_handler(int sig) {
    signal_count++;
    printf("Signal %d received (count: %d)\n", sig, signal_count);

    if (signal(sig, vulnerable_handler) == SIG_ERR) {
        printf("Error re-registering signal handler\n");
        exit(EXIT_FAILURE);
    }

    printf("Handler re-registered\n");
}

int main() {
    printf("Demonstrating vulnerable signal() call within handler\n");
    printf("Race condition window exists between handler start and signal() call\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, vulnerable_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send multiple SIGUSR1 signals rapidly to expose race condition\n");

    while (signal_count < 10) {
        pause();
    }

    printf("Received %d signals\n", signal_count);
    return 0;
}