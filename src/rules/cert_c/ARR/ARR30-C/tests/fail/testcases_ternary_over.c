/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Ternary operator used for array index without bounds validation
 */

#include <stdio.h>

int main(void) {
    int data[6] = {10, 20, 30, 40, 50, 60};
    int flag = 1;
    int large_index = 15;
    int small_index = 2;

    // Ternary may select out-of-bounds index
    int value = data[flag ? large_index : small_index];
    printf("Value: %d\n", value);

    // Assignment through ternary operator
    data[flag ? large_index : small_index] = 999;

    return 0;
}