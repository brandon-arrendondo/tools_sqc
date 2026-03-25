/*
 * Rule: DCL30-C
 * Source: testcases
 * Status: PASS - Safe pointer returns (heap or static storage)
 */

#include <stdlib.h>

/* Returning heap-allocated memory — safe */
int *return_heap(void) {
    int *p = (int *)malloc(sizeof(int));
    if (p) *p = 42;
    return p;
}

/* Returning static local — safe */
int *return_static(void) {
    static int x = 42;
    return &x;
}

/* Returning parameter pointer — safe (caller's storage) */
int *return_param(int *p) {
    return p;
}
