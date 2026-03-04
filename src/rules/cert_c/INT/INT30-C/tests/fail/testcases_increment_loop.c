/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Addition in loop condition without bounds check
 */

#include <limits.h>

void increment_no_check(unsigned int start) {
    unsigned int i;

    // Loop with unchecked unsigned addition in condition
    for (i = start; i < start + 100; i++) {
        // Process...
    }
}

int main(void) {
    increment_no_check(UINT_MAX - 50);
    return 0;
}
