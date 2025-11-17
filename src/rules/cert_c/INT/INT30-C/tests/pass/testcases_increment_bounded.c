/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Increment with bounds check
 */

#include <limits.h>

void increment_safe(unsigned int value) {
    unsigned int result;

    // Check before increment - COMPLIANT
    if (value == UINT_MAX) {
        // Handle error
        return;
    }

    result = value + 1;
    // Use result...
}

int main(void) {
    increment_safe(1000);
    return 0;
}
