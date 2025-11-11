/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: fopen (side effect) in unsafe macro
 */

#include <stdio.h>

#define IS_VALID_FILE(f) ((f) != NULL)  /* UNSAFE */

void open_file(void) {
    // fopen has side effect - may open file twice
    if (IS_VALID_FILE(fopen("test.txt", "r"))) {  // Line 13 - VIOLATION
        // File descriptor leak
    }
}

int main(void) {
    open_file();
    return 0;
}
