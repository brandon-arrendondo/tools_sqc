/*
 * Rule: INT31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT31-C violation
 * Description: Signed to unsigned with proper bounds check
 */

#include <limits.h>

void func(signed int si) {
    unsigned int ui;
    if (si < 0) {
        /* Handle error */
        return;
    } else {
        ui = (unsigned int)si;  /* Compliant: bounds checked */
    }
    /* ... */
    (void)ui;
}

void testcase_signed_to_unsigned_with_check(void) {
    func(INT_MIN + 1);
}
