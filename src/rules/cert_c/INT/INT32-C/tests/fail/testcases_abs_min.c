/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Taking absolute value of INT_MIN causes overflow
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int value = INT_MIN;
    int result = abs(value); // VIOLATION: abs(INT_MIN) overflows

    printf("Original: %d, Absolute: %d\n", value, result);
    return 0;
}