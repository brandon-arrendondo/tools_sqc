/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: I/O operation (getchar) in unsafe macro
 */

#include <stdio.h>

#define IS_UPPER(c) ((c) >= 'A' && (c) <= 'Z')  /* UNSAFE */

void read_input(void) {
    // getchar has side effect (I/O) - evaluated twice
    if (IS_UPPER(getchar())) {  // Line 12 - VIOLATION
        printf("Got uppercase\n");
    }
}

int main(void) {
    read_input();
    return 0;
}
