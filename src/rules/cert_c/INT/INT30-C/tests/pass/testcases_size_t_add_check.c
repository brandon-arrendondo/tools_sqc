/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: size_t addition with wrap check
 */

#include <stdlib.h>
#include <stddef.h>

void allocate_memory(size_t size1, size_t size2) {
    size_t total_size;

    // Check for addition wrap - COMPLIANT
    if (SIZE_MAX - size1 < size2) {
        // Handle error
        return;
    }

    total_size = size1 + size2;

    char *buffer = (char *)malloc(total_size);
    if (buffer) {
        free(buffer);
    }
}

int main(void) {
    allocate_memory(1000, 2000);
    return 0;
}
