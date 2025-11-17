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

void unsafe_printf_handler(int sig) {
    signal_count++;

    // Violation: Using non-async-safe function printf in signal handler
    // without proper signal masking allows reentrant calls
    printf("=== Signal %d received ===\n", sig);
    printf("This is signal number: %d\n", signal_count);

    // Make the vulnerability window larger
    for (int i = 0; i < 5; i++) {
        printf("Processing step %d...\n", i + 1);
        usleep(200000); // 200ms delay
    }

    printf("=== Signal %d handler complete ===\n", sig);
}

int main() {
    struct sigaction sa;

    // Install handler without masking signals
    sa.sa_handler = unsafe_printf_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Not masking signals during printf operations
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 and SIGUSR2 rapidly to see printf corruption\n");
    printf("The output may become garbled due to reentrancy\n\n");

    while (1) {
        printf("Main loop iteration... (count: %d)\n", signal_count);
        sleep(2);
    }

    return 0;
}