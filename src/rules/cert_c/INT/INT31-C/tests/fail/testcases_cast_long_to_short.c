/*
 * Rule: INT31-C
 * Status: FAIL - Implicit narrowing from long to char
 */

#include <stdio.h>

void f(long val) {
    char c = val;  /* VIOLATION: implicit narrowing from long to char */
    printf("%d\n", c);
}
