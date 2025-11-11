/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void memory_handler(int sig) {
    // VIOLATION: malloc() and free() are not async-safe
    char *buffer = malloc(256);
    if (buffer != NULL) {
        // Do some work with buffer
        buffer[0] = 'H';
        buffer[1] = '\0';
        free(buffer);
    }

    // VIOLATION: realloc() is not async-safe
    static char *static_buf = NULL;
    if (static_buf == NULL) {
        static_buf = malloc(100);
    } else {
        static_buf = realloc(static_buf, 200);
    }
}

int main() {
    printf("Demonstrating unsafe memory allocation in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, memory_handler);

    printf("Send SIGUSR1 to trigger unsafe memory operations\n");

    while (1) {
        pause();
    }

    return 0;
}