/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Original memory lost when realloc assigns to same pointer
 */

#include <stdlib.h>

void unsafe_realloc() {
    char *buffer = malloc(50);
    if (buffer == NULL) {
        return;
    }

    // Unsafe realloc - if it fails, original memory is leaked
    buffer = realloc(buffer, 100);
    if (buffer == NULL) {
        // Original 50-byte allocation is now leaked
        return;
    }

    // Use the buffer
    buffer[0] = 'X';

    free(buffer);  // This only frees if realloc succeeded
}