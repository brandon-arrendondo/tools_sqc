/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *emergency_buffer = NULL;

void memory_allocating_handler(int sig) {
    printf("Exception handler: Attempting memory allocation\n");

    /* Dangerous: allocating memory in signal handler */
    emergency_buffer = malloc(1024);

    if (emergency_buffer != NULL) {
        printf("Emergency buffer allocated at %p\n", emergency_buffer);
        /* Initialize some data */
        memset(emergency_buffer, 0xAA, 1024);
    } else {
        printf("Failed to allocate emergency buffer\n");
    }

    printf("Memory allocation complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing memory allocation in exception handler with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGSEGV, memory_allocating_handler);

    printf("Emergency buffer: %p\n", emergency_buffer);

    printf("Dereferencing null pointer...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("Emergency buffer after exception: %p\n", emergency_buffer);
    printf("This represents undefined behavior\n");

    if (emergency_buffer) free(emergency_buffer);
    return 0;
}