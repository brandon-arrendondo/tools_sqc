/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Memory is reallocated and finally freed, with proper error handling
 */

#include <stdlib.h>
#include <string.h>

void resize_buffer() {
    char *buffer = malloc(50);
    if (buffer == NULL) {
        return;
    }

    strcpy(buffer, "Initial data");

    // Resize the buffer
    char *temp = realloc(buffer, 100);
    if (temp == NULL) {
        free(buffer);  // Free original buffer on realloc failure
        return;
    }
    buffer = temp;

    strcat(buffer, " - extended data");

    // Properly free the final buffer
    free(buffer);
}