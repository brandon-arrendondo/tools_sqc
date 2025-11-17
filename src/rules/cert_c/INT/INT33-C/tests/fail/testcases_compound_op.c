/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Compound assignment operation with division by zero
 */

#include <stdio.h>

int main() {
    int x = 100;
    int divisor = 0;

    x /= divisor;  // Compound assignment: x = x / divisor (divide by zero)
    printf("Result: %d\n", x);
    return 0;
}