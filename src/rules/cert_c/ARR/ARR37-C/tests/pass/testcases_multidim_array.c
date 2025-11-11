/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Pointer arithmetic on multidimensional array
 */

#include <stdio.h>

void multidim_operations(void) {
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };

    // Pointer to first row - COMPLIANT
    int (*row_ptr)[4] = matrix;

    // Access rows with pointer arithmetic - COMPLIANT
    for (int i = 0; i < 3; i++) {
        int *col_ptr = *(row_ptr + i);
        for (int j = 0; j < 4; j++) {
            printf("%d ", *(col_ptr + j));
        }
        printf("\n");
    }
}

int main(void) {
    multidim_operations();
    return 0;
}
