/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: User input used as divisor without validation for zero
 */

#include <stdio.h>

int main() {
    int dividend, divisor;

    printf("Enter dividend: ");
    scanf("%d", &dividend);
    printf("Enter divisor: ");
    scanf("%d", &divisor);

    // No validation - user could enter 0
    int result = dividend / divisor;
    printf("Result: %d\n", result);
    return 0;
}