/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Pointer dereferencing to zero value used as divisor without validation
 */

#include <stdio.h>

int main() {
    int zero_value = 0;
    int *ptr = &zero_value;
    int dividend = 42;

    int result = dividend / (*ptr);  // Dereferencing pointer to zero
    printf("Result: %d\n", result);
    return 0;
}