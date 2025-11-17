/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Multiplication wrap in aligned_alloc call
 */

#include <stdlib.h>

void allocate_aligned(size_t count) {
    // Multiplication may wrap
    size_t size = count * sizeof(long long);  // Line 10 - VIOLATION

    void *ptr = aligned_alloc(16, size);
    if (ptr) {
        free(ptr);
    }
}

int main(void) {
    allocate_aligned(SIZE_MAX / 4);  // Will wrap
    return 0;
}
