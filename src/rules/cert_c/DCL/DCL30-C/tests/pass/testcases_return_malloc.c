/*
 * Rule: DCL30-C
 * Status: PASS - Returning heap-allocated memory (valid)
 */

#include <stdlib.h>

int *f(void) {
    int *p = malloc(sizeof(int));
    if (p) *p = 42;
    return p;  /* Safe: heap-allocated */
}
