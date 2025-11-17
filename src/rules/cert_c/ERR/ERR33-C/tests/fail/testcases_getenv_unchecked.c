/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: getenv() return value is not checked for NULL before use
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // VIOLATION: Return value not checked for NULL
    char *path = getenv("NONEXISTENT_VAR");

    // Direct use without NULL check
    printf("Length: %zu\n", strlen(path)); // Potential NULL pointer dereference

    // Another unchecked getenv call
    char *home = getenv("ANOTHER_NONEXISTENT_VAR");
    printf("Home: %s\n", home); // Potential NULL pointer dereference

    return 0;
}