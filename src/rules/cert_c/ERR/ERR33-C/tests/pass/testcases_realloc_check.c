/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: realloc() return value is properly checked to prevent memory leaks
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buffer = malloc(10);
    if (buffer == NULL) {
        fprintf(stderr, "Initial allocation failed\n");
        return 1;
    }

    // Safely reallocate memory
    char *temp = realloc(buffer, 20);
    if (temp == NULL) {
        fprintf(stderr, "Reallocation failed\n");
        free(buffer); // Don't leak the original memory
        return 1;
    }
    buffer = temp;

    // Use the reallocated memory
    buffer[0] = 'A';
    buffer[1] = '\0';
    printf("Buffer: %s\n", buffer);

    free(buffer);
    return 0;
}