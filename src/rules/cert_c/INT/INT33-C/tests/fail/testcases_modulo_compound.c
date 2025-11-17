/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Compound modulo assignment with zero divisor
 */

#include <stdio.h>

int main() {
    int x = 17;
    int divisor = 0;

    x %= divisor;  // Compound assignment: x = x % divisor (modulo by zero)
    printf("Result: %d\n", x);
    return 0;
}