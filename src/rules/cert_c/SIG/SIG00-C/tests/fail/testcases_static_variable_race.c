/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void static_handler(int sig) {
    static int call_count = 0;
    static char buffer[1024];

    printf("Handler: Signal %d, call #%d\n", sig, call_count + 1);

    // Violation: Static variables can be corrupted by signal interruption
    // without proper masking
    call_count++;

    // Simulate complex operation on static buffer
    for (int i = 0; i < 100; i++) {
        buffer[i] = 'A' + (i % 26);

        // Create vulnerability window
        if (i % 10 == 0) {
            usleep(1000);
        }
    }

    buffer[100] = '\0';

    printf("Handler: Buffer starts with: %.20s...\n", buffer);
    printf("Handler: Call count is now: %d\n", call_count);

    // Verify buffer integrity
    for (int i = 0; i < 100; i++) {
        if (buffer[i] != 'A' + (i % 26)) {
            printf("Handler: ERROR - Buffer corruption at position %d!\n", i);
            printf("Expected '%c', found '%c'\n", 'A' + (i % 26), buffer[i]);
            break;
        }
    }
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = static_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Static variables vulnerable to concurrent access
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 and SIGUSR2 rapidly to corrupt static variables\n");

    while (1) {
        sleep(1);
        printf("Main: Waiting for signals...\n");
    }

    return 0;
}