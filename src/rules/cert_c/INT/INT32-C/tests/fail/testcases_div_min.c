/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Dividing INT_MIN by -1 causes overflow
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int dividend = INT_MIN;
    int divisor = -1;
    int result = dividend / divisor; // VIOLATION: INT_MIN / -1 overflows

    printf("Result: %d\n", result);
    return 0;
}