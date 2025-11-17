/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using allocation size (bytes) as pointer offset
 */

#include <stdlib.h>

void malloc_size_offset(void) {
    size_t alloc_size = 200;
    int *buffer = (int *)malloc(alloc_size);

    if (buffer) {
        // alloc_size is bytes, gets scaled as int*
        int *end = buffer + alloc_size;  // Line 14 - VIOLATION
        *(end - 1) = 999;

        free(buffer);
    }
}

int main(void) {
    malloc_size_offset();
    return 0;
}
