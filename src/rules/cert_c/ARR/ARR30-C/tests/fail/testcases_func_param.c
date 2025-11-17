/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Function parameter array access without bounds validation
 */

#include <stdio.h>

void process_array(int arr[], int index) {
    // No bounds checking on index parameter
    printf("Value at index %d: %d\n", index, arr[index]);
    arr[index] = 999;
}

int main(void) {
    int data[3] = {10, 20, 30};

    // Pass out-of-bounds index
    process_array(data, 5);
    process_array(data, -1);

    return 0;
}