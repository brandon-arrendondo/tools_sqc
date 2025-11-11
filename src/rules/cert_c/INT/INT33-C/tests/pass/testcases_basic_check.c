/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Basic division with explicit zero check before performing the operation
 */

#include <stdio.h>

int safe_divide(int dividend, int divisor) {
    if (divisor == 0) {
        printf("Error: Division by zero\n");
        return -1;  // Error indicator
    }
    return dividend / divisor;
}

int main() {
    int a = 10, b = 2;
    int result = safe_divide(a, b);
    printf("Result: %d\n", result);
    return 0;
}