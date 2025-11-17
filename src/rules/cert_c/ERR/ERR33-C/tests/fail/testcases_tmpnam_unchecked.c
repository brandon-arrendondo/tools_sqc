/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: tmpnam() return value is not checked for NULL
 */

#include <stdio.h>

int main() {
    char buffer[L_tmpnam];

    // VIOLATION: Return value not checked for NULL
    tmpnam(buffer);

    // Assuming tmpnam succeeded
    printf("Temporary filename: %s\n", buffer); // May contain garbage on error

    // Another unchecked tmpnam with NULL parameter
    char *temp_name = tmpnam(NULL);
    printf("Another temp name: %s\n", temp_name); // Potential NULL dereference

    return 0;
}