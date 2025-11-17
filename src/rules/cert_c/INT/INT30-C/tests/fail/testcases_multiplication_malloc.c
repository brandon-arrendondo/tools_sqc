/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Multiplication in malloc without wrap check (real-world vulnerability pattern)
 */

#include <stdlib.h>
#include <stddef.h>

void alloc_buffer(size_t num_elements) {
    // Multiplication may wrap - insufficient allocation
    int *buffer = (int *)malloc(num_elements * sizeof(int));  // Line 11 - VIOLATION

    if (buffer) {
        // Use buffer...
        free(buffer);
    }
}

int main(void) {
    alloc_buffer(SIZE_MAX / 2);  // Will wrap
    return 0;
}
