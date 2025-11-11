/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t sigusr1_count = 0;
volatile sig_atomic_t sigusr2_count = 0;

void multi_handler(int sig) {
    switch (sig) {
        case SIGUSR1:
            sigusr1_count++;
            printf("SIGUSR1 count: %d\n", sigusr1_count);
            break;
        case SIGUSR2:
            sigusr2_count++;
            printf("SIGUSR2 count: %d\n", sigusr2_count);
            break;
    }
}

int main() {
    printf("FAIL: Multiple signals assuming handler persistence\n");

    /* Assumes both handlers will persist */
    signal(SIGUSR1, multi_handler);
    signal(SIGUSR2, multi_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 and SIGUSR2 alternately\n");

    /* Assumes handlers will remain registered */
    while (sigusr1_count < 3 || sigusr2_count < 3) {
        pause();
    }

    printf("SIGUSR1: %d, SIGUSR2: %d\n", sigusr1_count, sigusr2_count);
    printf("Code assumes all signal handlers persist\n");

    return 0;
}