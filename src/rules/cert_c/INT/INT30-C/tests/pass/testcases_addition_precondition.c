/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Addition with precondition check for wrap
 */

#include <limits.h>

void func(unsigned int ui_a, unsigned int ui_b) {
    unsigned int usum;

    // Precondition test - COMPLIANT
    if (UINT_MAX - ui_a < ui_b) {
        // Handle error
        return;
    }

    usum = ui_a + ui_b;
    // Use usum...
}

int main(void) {
    func(4000000000U, 1000000000U);
    return 0;
}
