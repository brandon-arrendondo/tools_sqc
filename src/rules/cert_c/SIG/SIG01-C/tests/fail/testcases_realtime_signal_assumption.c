/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t rt_signal_count = 0;

void rt_handler(int sig) {
    rt_signal_count++;
    printf("Real-time signal %d received (count: %d)\n", sig, rt_signal_count);
}

int main() {
    printf("FAIL: Real-time signal handling with persistence assumption\n");

#ifdef SIGRTMIN
    /* Assumes signal() works consistently with real-time signals */
    if (signal(SIGRTMIN + 1, rt_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Real-time signal %d registered\n", SIGRTMIN + 1);

    /* Send multiple real-time signals */
    int i;
    for (i = 0; i < 5; i++) {
        if (kill(getpid(), SIGRTMIN + 1) == -1) {
            perror("kill");
            break;
        }
        printf("Sent real-time signal %d\n", i + 1);
        usleep(200000);
    }

    sleep(1);

    printf("Real-time signals received: %d\n", rt_signal_count);
    printf("Assumes handler persists for real-time signals\n");
#else
    printf("Real-time signals not supported on this platform\n");
#endif

    return 0;
}