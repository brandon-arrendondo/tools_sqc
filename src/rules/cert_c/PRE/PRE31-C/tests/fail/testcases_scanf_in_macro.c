/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: I/O operation (scanf) in unsafe macro
 */

#include <stdio.h>

#define VALIDATE(x) ((x) > 0 && (x) < 100)  /* UNSAFE */

void get_input(void) {
    int value;

    // scanf has side effect (I/O) - evaluated twice
    if (VALIDATE(scanf("%d", &value))) {  // Line 14 - VIOLATION
        printf("Valid input\n");
    }
}

int main(void) {
    get_input();
    return 0;
}
