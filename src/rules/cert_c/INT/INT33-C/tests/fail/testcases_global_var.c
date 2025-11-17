/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Global variable with zero value used as divisor without validation
 */

#include <stdio.h>

int global_divisor = 0;  // Global variable initialized to zero

int divide_by_global(int dividend) {
    return dividend / global_divisor;  // No check for zero
}

int main() {
    int result = divide_by_global(50);
    printf("Result: %d\n", result);
    return 0;
}