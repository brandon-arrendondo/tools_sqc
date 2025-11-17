/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Increment in loop without bounds check
 */

#include <limits.h>

void increment_no_check(unsigned int start) {
    unsigned int i;

    // Increment without checking for UINT_MAX
    for (i = start; i < start + 100; i++) {  // Line 13 - VIOLATION (i++ can wrap)
        // Process...
    }
}

int main(void) {
    increment_no_check(UINT_MAX - 50);  // Will wrap
    return 0;
}
