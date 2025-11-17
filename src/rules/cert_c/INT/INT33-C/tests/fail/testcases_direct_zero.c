/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Direct division by zero literal without any checking
 */

#include <stdio.h>

int main() {
    int x = 10;
    int result = x / 0;  // Direct divide by zero
    printf("Result: %d\n", result);
    return 0;
}