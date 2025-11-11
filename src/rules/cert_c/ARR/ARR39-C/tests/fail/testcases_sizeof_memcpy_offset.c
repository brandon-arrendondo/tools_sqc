/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof-based offset in pointer for memcpy
 */

#include <string.h>
#include <stddef.h>

void memcpy_sizeof_offset(void) {
    int src[40] = {0};
    int dest[40];
    size_t offset_bytes = 20;  // Byte offset

    // Using byte offset with int pointer
    memcpy(dest + offset_bytes, src, 10 * sizeof(int));  // Line 14 - VIOLATION
}

int main(void) {
    memcpy_sizeof_offset();
    return 0;
}
