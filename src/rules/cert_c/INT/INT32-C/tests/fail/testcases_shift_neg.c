/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Left shifting negative values is undefined behavior and can cause overflow
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int value = -5;
    int result = value << 2; // VIOLATION: left shifting negative value is undefined

    printf("Result: %d\n", result);
    return 0;
}