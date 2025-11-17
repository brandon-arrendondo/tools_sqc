/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Array accessed via restrict-qualified pointer beyond bounds
 */

#include <stdio.h>

void process_array(int *restrict arr, int size) {
    // Violate bounds using restrict pointer
    arr[size] = 100;        // Line 11 - VIOLATION (index == size)
    arr[size + 5] = 200;    // Line 12 - VIOLATION
}

int main(void) {
    int data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    process_array(data, 8);

    // Direct violation with restrict pointer
    int *restrict ptr = data;
    ptr[10] = 999;          // Line 21 - VIOLATION

    return 0;
}
