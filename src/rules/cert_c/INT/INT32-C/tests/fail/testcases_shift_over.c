/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Left shifting causes overflow without proper checking
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int value = 1000000;
    int result = value << 10; // VIOLATION: 1000000 << 10 = 1024000000, may overflow

    printf("Result: %d\n", result);
    return 0;
}