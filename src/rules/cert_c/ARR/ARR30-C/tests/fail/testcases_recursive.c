/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Recursive function with array access that exceeds bounds
 */

#include <stdio.h>

void recursive_access(int arr[], int index, int depth) {
    if (depth > 10) return;

    // Access array without bounds checking
    printf("arr[%d] = %d (depth %d)\n", index, arr[index], depth);
    arr[index] = depth;

    // Recursive call with incremented index
    recursive_access(arr, index + 2, depth + 1);
}

int main(void) {
    int data[5] = {10, 20, 30, 40, 50};

    // Start recursive calls that will exceed bounds
    recursive_access(data, 0, 0);

    return 0;
}