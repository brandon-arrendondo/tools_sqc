/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers from two separate arrays
 */

#include <stddef.h>

void separate_arrays(void) {
    int array1[10] = {0};
    int array2[10] = {0};
    int *ptr1 = array1;
    int *ptr2 = array2;

    // Subtract pointers from different arrays
    ptrdiff_t diff = ptr2 - ptr1;  // Line 16 - VIOLATION
}

int main(void) {
    separate_arrays();
    return 0;
}
