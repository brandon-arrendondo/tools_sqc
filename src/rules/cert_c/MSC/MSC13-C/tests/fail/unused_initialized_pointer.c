/*
 * Rule: MSC13-C
 * Status: FAIL - Pointer initialized but never read
 */

#include <stdlib.h>

void f(void) {
    int *p = malloc(sizeof(int));  /* VIOLATION: p is never read */
}
