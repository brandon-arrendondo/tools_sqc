/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Early return bypasses free() call, causing memory leak
 */

#include <stdlib.h>

void risky_function(int condition) {
    char *buffer = malloc(1024);
    if (buffer == NULL) {
        return;
    }

    if (condition < 0) {
        return;  // Early return without freeing buffer - MEMORY LEAK
    }

    // Do some work
    buffer[0] = 'A';

    free(buffer);  // This is never reached if condition < 0
}