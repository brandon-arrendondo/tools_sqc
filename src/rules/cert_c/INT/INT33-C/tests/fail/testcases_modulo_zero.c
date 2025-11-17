/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Direct modulo by zero literal without any checking
 */

#include <stdio.h>

int main() {
    int x = 15;
    int remainder = x % 0;  // Direct modulo by zero
    printf("Remainder: %d\n", remainder);
    return 0;
}