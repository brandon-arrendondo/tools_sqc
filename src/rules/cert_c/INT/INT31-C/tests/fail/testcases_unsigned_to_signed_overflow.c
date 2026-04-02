/*
 * Rule: INT31-C
 * Status: FAIL - Unsigned to signed conversion may overflow
 */

#include <stdio.h>

void f(unsigned long ul) {
    int i = ul;  /* VIOLATION: unsigned long to int may overflow */
    printf("%d\n", i);
}
