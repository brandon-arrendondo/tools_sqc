/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Using wider type for intermediate calculation
 */

#include <stdint.h>

void func(uint32_t a, uint32_t b) {
    // Use wider type for calculation - COMPLIANT
    uint64_t result = (uint64_t)a + (uint64_t)b;

    if (result > UINT32_MAX) {
        // Handle overflow
        return;
    }

    uint32_t sum = (uint32_t)result;
    // Use sum...
}

int main(void) {
    func(4000000000U, 1000000000U);
    return 0;
}
