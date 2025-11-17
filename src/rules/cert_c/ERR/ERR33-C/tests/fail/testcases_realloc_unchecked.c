/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: realloc() return value is not checked, causing potential memory leak
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buffer = malloc(10);
    if (buffer == NULL) {
        return 1;
    }

    // VIOLATION: realloc return value not checked
    buffer = realloc(buffer, 20); // If this fails, original memory is leaked

    buffer[0] = 'A'; // Potential NULL pointer dereference
    buffer[1] = '\0';
    printf("Buffer: %s\n", buffer);

    free(buffer);
    return 0;
}