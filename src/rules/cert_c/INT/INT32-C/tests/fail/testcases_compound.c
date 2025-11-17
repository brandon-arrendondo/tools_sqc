/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Compound assignment operators can cause overflow
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int value1 = INT_MAX;
    int value2 = INT_MIN;
    int value3 = 1000000;

    printf("Initial values: %d, %d, %d\n", value1, value2, value3);

    // VIOLATION: compound addition overflow
    value1 += 1;

    // VIOLATION: compound subtraction underflow
    value2 -= 1;

    // VIOLATION: compound multiplication overflow
    value3 *= 3000;

    printf("After compound operations: %d, %d, %d\n", value1, value2, value3);
    return 0;
}