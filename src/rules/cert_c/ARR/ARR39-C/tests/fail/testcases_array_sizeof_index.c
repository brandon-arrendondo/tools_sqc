/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof as index multiplier
 */

#include <stddef.h>

void sizeof_as_index(void) {
    int matrix[10][10];
    int *ptr = &matrix[0][0];
    size_t row = 3;

    // Scaling row by sizeof(matrix[0]) - byte size
    int *row_ptr = ptr + (row * sizeof(matrix[0]));  // Line 14 - VIOLATION
    *row_ptr = 99;
}

int main(void) {
    sizeof_as_index();
    return 0;
}
