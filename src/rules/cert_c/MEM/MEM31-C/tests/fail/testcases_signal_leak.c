/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Signal handler interrupts execution, memory never freed
 */

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig) {
    printf("Signal received, terminating\n");
    exit(1);  // Exits without cleanup - MEMORY LEAK
}

void signal_prone_function() {
    signal(SIGINT, signal_handler);

    char *buffer = malloc(1024);
    if (buffer == NULL) {
        return;
    }

    buffer[0] = 'S';

    // Simulate long operation that might be interrupted
    sleep(10);  // If SIGINT received, signal_handler exits program

    free(buffer);  // This may never be reached if signal occurs
}