/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Array element containing zero used as divisor without checking
 */

#include <stdio.h>

int main() {
    int divisors[] = {2, 4, 0, 8, 10};  // Third element is zero
    int dividend = 100;

    for (int i = 0; i < 5; i++) {
        int result = dividend / divisors[i];  // No check for zero values
        printf("100 / %d = %d\n", divisors[i], result);
    }
    return 0;
}