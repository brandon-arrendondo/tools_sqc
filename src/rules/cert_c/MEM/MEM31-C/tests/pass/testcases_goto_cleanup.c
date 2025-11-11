/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Uses goto for cleanup to ensure memory is freed in all paths
 */

#include <stdlib.h>
#include <stdio.h>

int complex_function() {
    char *buffer1 = NULL;
    char *buffer2 = NULL;
    int result = -1;

    buffer1 = malloc(256);
    if (buffer1 == NULL) {
        goto cleanup;
    }

    buffer2 = malloc(512);
    if (buffer2 == NULL) {
        goto cleanup;
    }

    // Simulate some processing
    if (some_condition()) {
        result = 0;
        goto cleanup;
    }

    // More processing
    result = process_buffers(buffer1, buffer2);

cleanup:
    // Properly free all allocated memory
    free(buffer1);
    free(buffer2);
    return result;
}

int some_condition() { return 1; }
int process_buffers(char *b1, char *b2) { return 0; }