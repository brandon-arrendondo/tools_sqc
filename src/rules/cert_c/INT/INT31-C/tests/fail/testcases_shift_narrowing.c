/*
 * Rule: INT31-C
 * Status: FAIL - Unsigned long to int conversion
 */

#include <stdio.h>

void f(unsigned long ul) {
    int i = ul;  /* VIOLATION: unsigned long to int may overflow */
    printf("%d\n", i);
}
