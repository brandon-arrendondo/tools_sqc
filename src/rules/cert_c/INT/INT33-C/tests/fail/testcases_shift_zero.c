/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Bit shift operation results in zero used as divisor
 */

#include <stdio.h>

int main() {
    int value = 1;  // Binary: 00000001
    int dividend = 45;

    // Right shift by 8 positions makes small values become 0
    int divisor = value >> 8;  // 1 >> 8 = 0

    int result = dividend / divisor;  // Division by zero
    printf("Result: %d\n", result);
    return 0;
}