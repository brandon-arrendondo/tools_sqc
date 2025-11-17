/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int matrix[3][3] = {{1,2,3}, {4,5,6}, {7,8,9}};

    int value = matrix[0,2];

    printf("Value: %d\n", value);

    return 0;
}