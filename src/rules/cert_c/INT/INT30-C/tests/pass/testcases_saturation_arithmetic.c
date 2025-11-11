/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Saturation arithmetic instead of wrapping
 */

#include <limits.h>

unsigned int saturating_add(unsigned int a, unsigned int b) {
    unsigned int result;

    // Implement saturation - COMPLIANT
    if (UINT_MAX - a < b) {
        result = UINT_MAX;  // Saturate at maximum
    } else {
        result = a + b;
    }

    return result;
}

int main(void) {
    unsigned int sum = saturating_add(4000000000U, 1000000000U);
    // sum will be UINT_MAX, not wrapped
    return 0;
}
