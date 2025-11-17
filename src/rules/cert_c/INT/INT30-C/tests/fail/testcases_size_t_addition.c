/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: size_t addition without wrap check before allocation
 */

#include <stdlib.h>
#include <stddef.h>

void allocate_memory(size_t size1, size_t size2) {
    // Addition may wrap
    size_t total_size = size1 + size2;  // Line 11 - VIOLATION

    char *buffer = (char *)malloc(total_size);
    if (buffer) {
        free(buffer);
    }
}

int main(void) {
    allocate_memory(SIZE_MAX - 100, 200);  // Will wrap
    return 0;
}
