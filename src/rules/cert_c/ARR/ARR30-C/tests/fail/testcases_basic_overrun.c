/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Array access without bounds checking causes buffer overrun
 */

#include <stdio.h>

int main(void) {
    int arr[5] = {1, 2, 3, 4, 5};

    // Direct out-of-bounds access
    printf("arr[10] = %d\n", arr[10]);  // Reading beyond array bounds

    return 0;
}