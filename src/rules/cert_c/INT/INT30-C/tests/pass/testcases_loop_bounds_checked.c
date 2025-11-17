/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Loop with checked bounds to prevent wrap
 */

#include <limits.h>

void process_range(unsigned int start, unsigned int count) {
    unsigned int end;

    // Check for addition wrap - COMPLIANT
    if (UINT_MAX - start < count) {
        // Handle error
        return;
    }

    end = start + count;

    for (unsigned int i = start; i < end; i++) {
        // Process safely...
    }
}

int main(void) {
    process_range(100, 200);
    return 0;
}
