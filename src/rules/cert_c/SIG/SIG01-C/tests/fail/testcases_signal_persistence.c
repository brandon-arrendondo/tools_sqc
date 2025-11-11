/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void handler(int sig) {
    signal_count++;
    printf("Signal %d received (count: %d)\n", sig, signal_count);
}

int main() {
    printf("Using signal() - handler persistence may vary by platform\n");

    if (signal(SIGUSR1, handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 multiple times to test handler persistence\n");
    printf("On some systems, handler may be reset after first signal\n");

    while (signal_count < 5) {
        pause();
    }

    printf("Received %d signals total\n", signal_count);
    return 0;
}