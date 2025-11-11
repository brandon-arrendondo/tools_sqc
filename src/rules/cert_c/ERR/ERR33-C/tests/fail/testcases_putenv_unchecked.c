/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: putenv() return value is not checked for failure (non-zero)
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    // VIOLATION: Return value not checked
    putenv("TEST_VAR=test_value");

    printf("Environment variable supposedly set\n");

    // Another unchecked putenv
    putenv("ANOTHER_VAR=another_value");
    printf("Another variable supposedly set\n");

    // Try to use the supposedly set variable
    char *value = getenv("TEST_VAR");
    if (value != NULL) {
        printf("TEST_VAR: %s\n", value);
    }

    return 0;
}