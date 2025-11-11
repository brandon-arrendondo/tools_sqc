/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Pointer arithmetic within flattened view of multidimensional array
 */

#include <stddef.h>
#include <stdio.h>

void multidim_array(void) {
    int matrix[5][10];
    int *ptr1 = &matrix[0][0];
    int *ptr2 = &matrix[4][9];

    // In memory, multidim array is contiguous, pointers refer to same object - COMPLIANT
    ptrdiff_t total_elements = ptr2 - ptr1 + 1;
    printf("Total elements between: %td\n", total_elements);

    if (ptr1 < ptr2) {
        printf("Valid comparison within same array object\n");
    }
}

int main(void) {
    multidim_array();
    return 0;
}
