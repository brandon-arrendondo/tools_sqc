/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Conditional expression can result in zero divisor without proper checking
 */

#include <stdio.h>

int main() {
    int flag = 1;
    int dividend = 30;
    int divisor = flag ? 0 : 5;  // When flag is 1, divisor becomes 0

    int result = dividend / divisor;  // No validation
    printf("Result: %d\n", result);
    return 0;
}