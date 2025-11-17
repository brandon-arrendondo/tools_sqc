/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t lost_signals = 0;

void storm_handler(int sig) {
    int old_count = signal_count;
    signal_count++;

    printf("Handler entry: count was %d, now %d\n", old_count, signal_count);

    for (int i = 0; i < 1000000; i++) {
        volatile int dummy = i * 2;
    }

    if (signal_count != old_count + 1) {
        lost_signals++;
        printf("WARNING: Signal count inconsistent! Lost signals: %d\n", lost_signals);
    }

    printf("Handler exit: count = %d\n", signal_count);
}

int main() {
    signal(SIGUSR1, storm_handler);

    printf("PID: %d\n", getpid());
    printf("Send rapid SIGUSR1 signals to create signal storm\n");

    while (signal_count < 50) {
        pause();
    }

    printf("Final count: %d, Lost: %d\n", signal_count, lost_signals);
    return 0;
}