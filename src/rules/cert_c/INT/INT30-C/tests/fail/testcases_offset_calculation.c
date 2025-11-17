/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Multiple additions without wrap check
 */

#include <stddef.h>

void calculate_offset(size_t base, size_t offset1, size_t offset2, size_t offset3) {
    // Multiple additions - any may wrap
    size_t total = base + offset1 + offset2 + offset3;  // Line 11 - VIOLATION

    // Use total for file seeking or memory access...
}

int main(void) {
    calculate_offset(SIZE_MAX / 2, SIZE_MAX / 4, SIZE_MAX / 4, 100);
    return 0;
}
