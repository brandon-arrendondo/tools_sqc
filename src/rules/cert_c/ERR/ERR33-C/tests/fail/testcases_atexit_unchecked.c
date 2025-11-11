/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: atexit() return value is not checked for failure (non-zero)
 */

#include <stdio.h>
#include <stdlib.h>

void cleanup_function() {
    printf("Cleanup function called\n");
}

void another_cleanup() {
    printf("Another cleanup called\n");
}

int main() {
    // VIOLATION: Return value not checked
    atexit(cleanup_function);

    printf("Exit handler supposedly registered\n");

    // Another unchecked atexit
    atexit(another_cleanup);
    printf("Another exit handler supposedly registered\n");

    return 0;
}