/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fopen() return value is not checked for NULL before use
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("nonexistent.txt", "r"); // VIOLATION: No NULL check

    char buffer[256];
    fgets(buffer, sizeof(buffer), file); // Potential NULL pointer dereference
    printf("Read: %s", buffer);

    fclose(file); // Another potential NULL pointer dereference
    return 0;
}