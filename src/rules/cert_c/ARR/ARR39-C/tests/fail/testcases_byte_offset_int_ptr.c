/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Adding byte offset to int pointer causes double-scaling
 */

#include <stddef.h>

void byte_offset(void) {
    int data[40];
    size_t byte_offset = 16;  // Byte offset

    // Adding byte offset to int* - will be scaled by sizeof(int)
    int *ptr = data + byte_offset;  // Line 14 - VIOLATION
    *ptr = 55;
}

int main(void) {
    byte_offset();
    return 0;
}
