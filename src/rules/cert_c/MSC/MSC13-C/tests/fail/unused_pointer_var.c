/*
 * Rule: MSC13-C
 * Status: FAIL - Pointer variable declared but never used
 */

#include <stdlib.h>

void f(void) {
    int *p;  /* VIOLATION: p is never used */
}
