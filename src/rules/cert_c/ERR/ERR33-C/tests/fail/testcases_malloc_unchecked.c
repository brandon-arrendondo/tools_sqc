/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: malloc() return value is not checked for NULL before use
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buffer = malloc(1024); // VIOLATION: No NULL check

    // Direct use without checking if allocation succeeded
    buffer[0] = 'H'; // Potential NULL pointer dereference
    buffer[1] = '\0';
    printf("Buffer: %s\n", buffer);

    free(buffer);
    return 0;
}