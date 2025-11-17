/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *temp_file = NULL;
void *allocated_memory = NULL;

void cleanup_handler(int sig) {
    printf("Exception handler: Performing cleanup operations\n");

    /* Attempt to clean up resources */
    if (temp_file != NULL) {
        printf("Closing temporary file\n");
        fclose(temp_file);
        temp_file = NULL;
    }

    if (allocated_memory != NULL) {
        printf("Freeing allocated memory\n");
        free(allocated_memory);
        allocated_memory = NULL;
    }

    printf("Cleanup completed, returning to continue (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing cleanup operations with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGSEGV, cleanup_handler);

    /* Allocate some resources */
    temp_file = fopen("/tmp/claude/temp.txt", "w");
    allocated_memory = malloc(1024);

    printf("Resources allocated, triggering segmentation fault...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This represents undefined behavior\n");
    return 0;
}