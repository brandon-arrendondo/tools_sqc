/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Loop counter increment can overflow when reaching INT_MAX
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int count = 0;

    // VIOLATION: No check for overflow in loop increment
    for (int i = INT_MAX - 2; i <= INT_MAX; i++) {
        count++; // This loop will overflow when i++ happens at INT_MAX
        printf("i = %d, count = %d\n", i, count);
    }

    return 0;
}