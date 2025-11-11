/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Pointer arithmetic creates out-of-bounds pointer
 */

#include <stdio.h>

int main(void) {
    int arr[5] = {1, 2, 3, 4, 5};
    int *ptr = arr;

    // Advancing pointer beyond array bounds
    ptr += 10;

    // Dereferencing out-of-bounds pointer
    printf("Value: %d\n", *ptr);

    return 0;
}