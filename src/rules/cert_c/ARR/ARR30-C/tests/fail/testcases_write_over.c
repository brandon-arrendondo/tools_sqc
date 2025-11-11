/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Writing beyond array bounds corrupts adjacent memory
 */

#include <stdio.h>

int main(void) {
    int arr1[5] = {1, 2, 3, 4, 5};
    int arr2[5] = {6, 7, 8, 9, 10};

    printf("Before: arr2[0] = %d\n", arr2[0]);

    // Writing beyond arr1 bounds may corrupt arr2
    arr1[5] = 999;
    arr1[6] = 888;

    printf("After: arr2[0] = %d\n", arr2[0]);
    return 0;
}