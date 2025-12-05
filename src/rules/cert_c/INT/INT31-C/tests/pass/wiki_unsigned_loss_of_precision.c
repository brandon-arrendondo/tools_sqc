/*
 * Rule: INT31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT31-C violation
 * Description: Narrowing conversion with proper bounds check
 */

#include <limits.h>

void testcase_unsigned_narrowing_with_check(void) {
    unsigned long int u_a = ULONG_MAX;
    unsigned char uc;
    if (u_a > UCHAR_MAX) {
        /* Handle error */
        return;
    } else {
        uc = (unsigned char)u_a;  /* Compliant: bounds checked */
    }
    /* ... */
    (void)uc;
}
