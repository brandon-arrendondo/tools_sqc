/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Expression evaluation results in zero used as divisor
 */

#include <stdio.h>

int main() {
    int a = 5, b = 5;
    int dividend = 20;

    int result = dividend / (a - b);  // Expression evaluates to 0
    printf("20 / (5 - 5) = %d\n", result);
    return 0;
}