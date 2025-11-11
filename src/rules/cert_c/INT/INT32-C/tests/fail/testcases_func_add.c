/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Function performs addition without checking for overflow
 */

#include <limits.h>
#include <stdio.h>

int add_values(int a, int b) {
    return a + b; // VIOLATION: no overflow check
}

int main() {
    int result1 = add_values(INT_MAX, 1);
    int result2 = add_values(INT_MIN, -1);

    printf("Result 1: %d\n", result1);
    printf("Result 2: %d\n", result2);

    return 0;
}