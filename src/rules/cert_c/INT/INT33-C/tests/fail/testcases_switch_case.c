/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Switch case allows zero value to be used as divisor
 */

#include <stdio.h>

int main() {
    int operation = 0;  // User selects division by zero case
    int dividend = 15;
    int divisor;

    switch (operation) {
        case 0:
            divisor = 0;  // Explicitly set to zero
            break;
        case 1:
            divisor = 3;
            break;
        default:
            divisor = 1;
            break;
    }

    int result = dividend / divisor;  // No validation before division
    printf("Result: %d\n", result);
    return 0;
}