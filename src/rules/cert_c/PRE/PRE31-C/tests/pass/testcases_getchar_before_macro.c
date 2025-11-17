/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: I/O operation separate from macro call
 */

#include <stdio.h>

#define IS_UPPER(c) ((c) >= 'A' && (c) <= 'Z')  /* UNSAFE */

void read_input(void) {
    // I/O before macro call - COMPLIANT
    int ch = getchar();

    if (IS_UPPER(ch)) {
        printf("Got uppercase\n");
    }
}

int main(void) {
    read_input();
    return 0;
}
