/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Multiplication for buffer size calculation without wrap check
 */

#include <stddef.h>

void calculate_buffer_size(size_t num_rows, size_t num_cols) {
    // Multiplication may wrap
    size_t buffer_size = num_rows * num_cols;  // Line 11 - VIOLATION

    // Use buffer_size for allocation...
}

int main(void) {
    calculate_buffer_size(SIZE_MAX / 100, 200);  // Will wrap
    return 0;
}
