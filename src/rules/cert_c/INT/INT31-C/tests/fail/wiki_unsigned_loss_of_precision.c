/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 * Description: Loss of precision in narrowing conversion
 */

#include <limits.h>

void testcase_unsigned_narrowing_no_check(void) {
    unsigned long int u_a = ULONG_MAX;
    unsigned char uc = (unsigned char)u_a;  /* Violation: value truncated */
    /* ... */
    (void)uc;
}
