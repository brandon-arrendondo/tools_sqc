/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: Memory allocation return value is properly checked for failure
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buffer = malloc(1024);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Use the allocated memory
    buffer[0] = 'H';
    buffer[1] = '\0';
    printf("Buffer: %s\n", buffer);

    free(buffer);
    return 0;
}