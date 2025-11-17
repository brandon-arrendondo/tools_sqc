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

volatile char* allocated_memory = NULL;

void dangerous_malloc_handler(int sig) {
    printf("Handler: Attempting memory allocation\n");

    // Violation: Using malloc (non-async-safe) in signal handler
    // without proper signal masking
    char* buffer = malloc(1024);
    if (buffer == NULL) {
        printf("Handler: malloc failed\n");
        return;
    }

    snprintf(buffer, 1024, "Signal %d data", sig);

    // This creates a race condition if interrupted
    if (allocated_memory != NULL) {
        free((void*)allocated_memory);
    }

    allocated_memory = buffer;

    // Delay to increase chance of interruption
    sleep(1);

    printf("Handler: Memory allocated and assigned\n");
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = dangerous_malloc_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: malloc can be interrupted by signal, causing corruption
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to trigger malloc in handler\n");
    printf("This may cause heap corruption\n");

    for (int i = 0; i < 100; i++) {
        printf("Main: iteration %d\n", i);

        // Also allocate in main to increase chance of corruption
        char* main_buffer = malloc(512);
        if (main_buffer) {
            strcpy(main_buffer, "Main thread data");
            free(main_buffer);
        }

        sleep(1);
    }

    if (allocated_memory) {
        free((void*)allocated_memory);
    }

    return 0;
}