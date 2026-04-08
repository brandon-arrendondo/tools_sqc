/*
 * Cross-file header test — implementation file.
 * Defines functions prototyped in public_api.h.
 * With -d, prescan sees the .h prototype and DCL15-C should NOT
 * flag compute_value() or print_result() as needing static.
 *
 * internal_helper() has no header prototype and should still be
 * flagged by DCL15-C as a candidate for static linkage.
 */

#include <stdio.h>

int compute_value(int x) {
    return x * 2 + 1;
}

void print_result(int value) {
    printf("Result: %d\n", value);
}

/* No header prototype — DCL15-C should flag this */
void internal_helper(void) {
    printf("helper\n");
}
