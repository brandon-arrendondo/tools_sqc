/*
 * Rule: INT31-C
 * Status: PASS - Non-negative guard before signed-to-unsigned conversion
 */

#include <stdlib.h>

void f(int n) {
    if (n >= 0) {
        char *buf = malloc(n);  /* Safe: n is known non-negative */
        if (buf) free(buf);
    }
}
