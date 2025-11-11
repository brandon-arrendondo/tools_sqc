/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Division by variable that is zero without proper checking
 */

#include <stdio.h>

int main() {
    int dividend = 20;
    int divisor = 0;  // Variable initialized to zero
    int result = dividend / divisor;  // No check before division
    printf("Result: %d\n", result);
    return 0;
}