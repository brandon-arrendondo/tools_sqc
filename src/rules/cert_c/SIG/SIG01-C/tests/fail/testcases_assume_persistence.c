/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t ready = 0;

void setup_handler(int sig) {
    ready = 1;
    printf("Setup complete, ready for operations\n");
}

int main() {
    printf("Dangerous assumption: signal handler will persist\n");

    signal(SIGUSR1, setup_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 to initialize, then SIGUSR1 again for operation\n");

    while (!ready) {
        pause();
    }

    printf("Ready! Send another SIGUSR1 for operation...\n");

    pause();

    if (ready == 2) {
        printf("Operation completed successfully\n");
    } else {
        printf("ERROR: Handler may have been reset - operation failed\n");
    }

    return 0;
}