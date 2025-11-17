/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Multiplication check before malloc
 */

#include <stdlib.h>
#include <stddef.h>

void allocate_buffer(size_t num_elements) {
    // Check for multiplication wrap - COMPLIANT
    if (num_elements > SIZE_MAX / sizeof(int)) {
        // Handle error
        return;
    }

    int *buffer = (int *)malloc(num_elements * sizeof(int));

    if (buffer) {
        // Use buffer...
        free(buffer);
    }
}

int main(void) {
    allocate_buffer(1000);
    return 0;
}
