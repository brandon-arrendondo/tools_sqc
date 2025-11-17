/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Logical operation result used as divisor without checking for zero
 */

#include <stdio.h>

int main() {
    int a = 5, b = 10;
    int dividend = 30;

    // Logical AND returns 0 when one operand is false
    int divisor = (a > 10) && (b > 5);  // false && true = 0

    int result = dividend / divisor;  // Division by zero
    printf("Result: %d\n", result);
    return 0;
}