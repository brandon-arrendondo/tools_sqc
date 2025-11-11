/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Static variable initialized to zero used as divisor
 */

#include <stdio.h>

static int static_divisor = 0;  // Static variable with zero

int divide_by_static(int dividend) {
    return dividend / static_divisor;  // No validation
}

int main() {
    int result = divide_by_static(72);
    printf("Result: %d\n", result);
    return 0;
}