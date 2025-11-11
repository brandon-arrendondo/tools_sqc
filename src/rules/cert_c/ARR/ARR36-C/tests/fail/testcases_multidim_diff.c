/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers from different rows of separate 2D arrays
 */

#include <stddef.h>

void multidim(void) {
    int matrix1[3][4];
    int matrix2[3][4];

    int *ptr1 = matrix1[0];
    int *ptr2 = matrix2[1];

    // Subtract pointers from different 2D arrays
    ptrdiff_t diff = ptr2 - ptr1;  // Line 17 - VIOLATION
}

int main(void) {
    multidim();
    return 0;
}
