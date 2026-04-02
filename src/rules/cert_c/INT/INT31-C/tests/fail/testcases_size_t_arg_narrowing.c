/*
 * Rule: INT31-C
 * Status: FAIL - Narrowing conversion when passing to size_t parameter
 */

#include <stdlib.h>

void f(int n) {
    /* int to size_t: signed-to-unsigned conversion */
    char *buf = malloc(n);  /* VIOLATION if n is negative */
}
