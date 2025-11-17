/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting restrict-qualified pointers to different arrays
 */

#include <stddef.h>

void restrict_subtract(int *restrict arr1, int *restrict arr2, int n) {
    int *ptr1 = arr1;
    int *ptr2 = arr2;

    // Subtract pointers from different restrict arrays
    ptrdiff_t diff = ptr2 - ptr1;  // Line 14 - VIOLATION
}

int main(void) {
    int a[20] = {0};
    int b[20] = {0};
    restrict_subtract(a, b, 20);
    return 0;
}
