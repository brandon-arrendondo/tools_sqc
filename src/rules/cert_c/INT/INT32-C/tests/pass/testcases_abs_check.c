/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Absolute value operation checks for INT_MIN before taking absolute value to avoid overflow
 */

#include <limits.h>
#include <stdio.h>

int safe_abs(int a, int *result) {
    if (a == INT_MIN) {
        return -1; // abs(INT_MIN) would overflow
    }
    *result = (a < 0) ? -a : a;
    return 0;
}

int main() {
    int result;
    int values[] = {-42, 0, 42, -100, INT_MIN, INT_MAX};
    int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        if (safe_abs(values[i], &result) == 0) {
            printf("abs(%d) = %d\n", values[i], result);
        } else {
            printf("abs(%d) would overflow\n", values[i]);
        }
    }

    return 0;
}