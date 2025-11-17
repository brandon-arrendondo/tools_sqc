/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: goto statement bypasses bounds checking logic
 */

#include <stdio.h>

int main(void) {
    int arr[5] = {1, 2, 3, 4, 5};
    int index = 8;

    if (index >= 0 && index < 5) {
        printf("Safe access: arr[%d] = %d\n", index, arr[index]);
    } else {
        goto unsafe_access;  // Jump to unsafe code
    }

    return 0;

unsafe_access:
    // This bypasses the bounds check above
    printf("Unsafe access: arr[%d] = %d\n", index, arr[index]);
    arr[index] = 999;
    return 0;
}