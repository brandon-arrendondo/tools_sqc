/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Program terminates with exit/abort without freeing memory
 */

#include <stdlib.h>

void critical_function() {
    char *buffer = malloc(1024);
    if (buffer == NULL) {
        return;
    }

    // Simulate critical error condition
    if (critical_error_detected()) {
        printf("Critical error - aborting\n");
        abort();  // Program terminates without freeing buffer - MEMORY LEAK
    }

    buffer[0] = 'X';
    free(buffer);
}

int critical_error_detected() {
    return 1;  // Simulate error
}