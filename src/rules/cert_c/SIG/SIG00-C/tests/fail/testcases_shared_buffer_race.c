/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256

volatile char shared_buffer[BUFFER_SIZE];
volatile int buffer_index = 0;

void buffer_handler(int sig) {
    char msg[64];
    snprintf(msg, sizeof(msg), "Signal %d received ", sig);

    // Vulnerable: Writing to shared buffer without protection
    int len = strlen(msg);
    for (int i = 0; i < len && buffer_index < BUFFER_SIZE - 1; i++) {
        shared_buffer[buffer_index++] = msg[i];

        // Create window for race condition
        if (i % 5 == 0) {
            usleep(1000);
        }
    }

    shared_buffer[buffer_index] = '\0';
    printf("Buffer content: %s\n", (char*)shared_buffer);
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = buffer_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Multiple signals can corrupt shared buffer
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send multiple signals quickly to corrupt buffer\n");

    while (1) {
        sleep(5);
        printf("Current buffer: %s\n", (char*)shared_buffer);
        buffer_index = 0; // Reset for next test
        memset((char*)shared_buffer, 0, BUFFER_SIZE);
    }

    return 0;
}