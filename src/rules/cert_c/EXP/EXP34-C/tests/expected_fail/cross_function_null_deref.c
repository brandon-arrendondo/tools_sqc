/*
 * Rule: EXP34-C
 * Status: EXPECTED_FAIL - Known limitation: intra-file null deref without
 *         call-site analysis. Requires inter-procedural null propagation
 *         through function return values within a single translation unit.
 */

#include <stdlib.h>

static int *get_possibly_null(void) {
    return NULL;
}

void caller(void) {
    int *p = get_possibly_null();
    /* No null check — dereference of possibly-null return value */
    *p = 42;
}
