/*
 * Rule: EXP19-C
 * Source: testcases
 * Status: PASS - No side effects in macro arguments
 */

#include <assert.h>

/* assert with pure comparison */
void check_positive(int x) {
    assert(x > 0);
}

/* assert with constant */
void check_true(void) {
    assert(1);
}

/* assert with variable comparison */
void check_bounds(int x, int max) {
    assert(x < max);
}
