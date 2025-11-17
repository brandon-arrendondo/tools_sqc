/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Signal handler frees memory that main function later accesses
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int *shared_data;

void signal_handler(int sig) {
    printf("Signal received, cleaning up\n");
    free(shared_data);
    shared_data = NULL;
}

int main() {
    shared_data = malloc(sizeof(int));
    if (shared_data == NULL) {
        return -1;
    }

    *shared_data = 111;
    signal(SIGTERM, signal_handler);

    // Simulate signal being raised
    raise(SIGTERM);

    // BUG: Access after signal handler freed it
    printf("Data: %d\n", *shared_data);

    return 0;
}