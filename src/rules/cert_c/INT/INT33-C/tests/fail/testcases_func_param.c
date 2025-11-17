/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Function parameter used as divisor without validation
 */

#include <stdio.h>

int divide_numbers(int a, int b) {
    return a / b;  // No check if b is zero
}

int main() {
    int result = divide_numbers(10, 0);  // Passing zero as divisor
    printf("Result: %d\n", result);
    return 0;
}