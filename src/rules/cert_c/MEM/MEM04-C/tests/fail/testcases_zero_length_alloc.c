/*
 * Rule: MEM04-C
 * Source: testcases
 * Status: FAIL - Zero-length allocation
 */

#include <stdlib.h>

/* malloc(0) — implementation-defined behavior */
void zero_alloc(void) {
    void *p = malloc(0);
    free(p);
}

/* calloc with zero count */
void zero_calloc(void) {
    void *p = calloc(0, sizeof(int));
    free(p);
}
