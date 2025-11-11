/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Addition operation includes proper overflow checking before performing the operation
 */

#include <limits.h>
#include <stdio.h>

int safe_add(int a, int b, int *result) {
    if (a > 0 && b > 0 && a > INT_MAX - b) {
        return -1; // Positive overflow
    }
    if (a < 0 && b < 0 && a < INT_MIN - b) {
        return -1; // Negative overflow
    }
    *result = a + b;
    return 0;
}

int main() {
    int result;
    if (safe_add(INT_MAX - 1, 1, &result) == 0) {
        printf("Result: %d\n", result);
    } else {
        printf("Addition would overflow\n");
    }
    return 0;
}