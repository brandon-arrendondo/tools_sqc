/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: sizeof with casted pointer arithmetic
 */

#include <stdlib.h>

void cast_sizeof(void) {
    void *buffer = malloc(400);

    if (buffer) {
        // Cast to long* and use sizeof
        long *ptr = (long *)buffer;
        long *offset_ptr = ptr + sizeof(long) * 10;  // Line 14 - VIOLATION

        free(buffer);
    }
}

int main(void) {
    cast_sizeof();
    return 0;
}
