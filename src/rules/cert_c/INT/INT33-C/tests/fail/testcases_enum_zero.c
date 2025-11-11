/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Enum value of zero used as divisor without validation
 */

#include <stdio.h>

enum Numbers {
    ZERO = 0,
    ONE = 1,
    TWO = 2
};

int main() {
    enum Numbers divisor = ZERO;  // Enum value is zero
    int dividend = 40;

    int result = dividend / divisor;  // No check for zero enum value
    printf("Result: %d\n", result);
    return 0;
}