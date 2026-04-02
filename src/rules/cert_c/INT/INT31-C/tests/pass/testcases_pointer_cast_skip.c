/*
 * Rule: INT31-C
 * Status: PASS - Pointer casts should not be flagged (pointer reinterpretation)
 */

#include <stdlib.h>

void f(void) {
    void *p = malloc(100);
    int *ip = (int *)p;       /* Pointer cast, not value conversion */
    char *cp = (char *)ip;    /* Pointer cast, not value conversion */
    free(cp);
}
