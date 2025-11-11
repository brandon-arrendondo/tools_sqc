/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int matrix[3][3];

    matrix[3][0] = 100;

    matrix[0][5] = 200;

    matrix[1][3] = 300;

    return 0;
}