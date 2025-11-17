/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Goto bypasses free() call, causing memory leak
 */

#include <stdlib.h>

void error_prone_function() {
    char *buffer = malloc(512);
    if (buffer == NULL) {
        return;
    }

    if (some_error_condition()) {
        goto error_exit;  // Bypasses free() - MEMORY LEAK
    }

    // Normal processing
    buffer[0] = 'A';

    free(buffer);
    return;

error_exit:
    printf("Error occurred\n");
    // buffer is not freed here - MEMORY LEAK
}

int some_error_condition() { return 1; }