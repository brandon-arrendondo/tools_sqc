/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t usr1_count = 0;
volatile sig_atomic_t usr2_count = 0;

void usr1_handler(int sig) {
    usr1_count++;
    printf("SIGUSR1 received (count: %d)\n", usr1_count);
}

void usr2_handler(int sig) {
    usr2_count++;
    printf("SIGUSR2 received (count: %d)\n", usr2_count);
}

int main() {
    struct sigaction sa1, sa2;
    printf("Safe signal handling without modifying handlers\n");
    printf("PID: %d\n", getpid());

    sa1.sa_handler = usr1_handler;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;

    sa2.sa_handler = usr2_handler;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa1, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa2, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("Handlers installed once and remain persistent\n");
    printf("Send SIGUSR1 and SIGUSR2 signals\n");

    while (usr1_count + usr2_count < 10) {
        pause();
    }

    printf("Total signals received: SIGUSR1=%d, SIGUSR2=%d\n",
           usr1_count, usr2_count);
    return 0;
}