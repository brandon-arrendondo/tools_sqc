/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Negative array index creates out-of-bounds access
 */

#include <stdio.h>

int main(void) {
    int data[8] = {10, 20, 30, 40, 50, 60, 70, 80};

    // Using negative index to access memory before array
    int value = data[-1];
    printf("data[-1] = %d\n", value);

    // Also accessing with negative offset
    data[-2] = 999;

    return 0;
}