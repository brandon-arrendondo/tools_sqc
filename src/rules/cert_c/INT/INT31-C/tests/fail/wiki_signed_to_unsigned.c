/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 * Description: Signed to unsigned conversion without bounds check
 */

#include <limits.h>

void func(signed int si) {
    /* Cast eliminates warning but allows negative values */
    unsigned int ui = (unsigned int)si;  /* Violation: no bounds check */

    /* ... */
    (void)ui;
}

void testcase_signed_to_unsigned_no_check(void) {
    func(INT_MIN);
}
