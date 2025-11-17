/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Pre-increment without bounds check
 */

#include <limits.h>

void pre_increment_unsafe(unsigned int value) {
    // Pre-increment without checking UINT_MAX
    unsigned int result = ++value;  // Line 11 - VIOLATION

    // Use result...
}

int main(void) {
    pre_increment_unsafe(UINT_MAX);  // Will wrap to 0
    return 0;
}
