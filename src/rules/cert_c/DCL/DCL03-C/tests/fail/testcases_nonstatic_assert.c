/*
 * Rule: DCL03-C
 * Source: testcases
 * Status: FAIL - Assertions on constant expressions should use static_assert
 */

#include <assert.h>

/* Runtime assert on compile-time constant */
void check_sizes(void) {
    assert(sizeof(int) == 4);
    assert(sizeof(long) >= 4);
}

/* Runtime assert on literal constant */
void check_constants(void) {
    assert(8 > 0);
    assert(256 <= 65536);
}
