/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: time() (side effect) in unsafe macro
 */

#include <time.h>

#define IS_POSITIVE(x) ((x) > 0)  /* UNSAFE */

void check_time(void) {
    // time() has side effect (system call) - may be called twice
    if (IS_POSITIVE(time(NULL))) {  // Line 13 - VIOLATION
        // Unexpected behavior
    }
}

int main(void) {
    check_time();
    return 0;
}
