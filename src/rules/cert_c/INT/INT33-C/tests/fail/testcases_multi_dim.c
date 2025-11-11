/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Multi-dimensional array element with zero used as divisor
 */

#include <stdio.h>

int main() {
    int matrix[2][2] = {{1, 2}, {3, 0}};  // Last element is zero
    int dividend = 96;

    int result = dividend / matrix[1][1];  // Division by zero element
    printf("Result: %d\n", result);
    return 0;
}