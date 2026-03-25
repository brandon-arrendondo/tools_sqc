/*
 * Rule: DCL30-C
 * Source: testcases
 * Status: PASS - Safe return patterns (heap, static, params)
 */

#include <stdlib.h>

/* Return malloc'd memory */
char *return_malloc(int n) {
    char *buf = (char *)malloc(n);
    return buf;
}

/* Return static local */
const char *return_static(void) {
    static char buf[256] = "hello";
    return buf;
}

/* Return parameter pointer (not local) */
int *return_param(int *arr) {
    return arr;
}

/* Return NULL is safe */
char *return_null(void) {
    return NULL;
}

/* Return string literal (static storage) */
const char *return_literal(void) {
    return "hello";
}

/* No return of local */
void no_return(void) {
    int x = 42;
    (void)x;
}
