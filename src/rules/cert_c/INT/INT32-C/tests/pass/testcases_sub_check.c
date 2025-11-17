/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Subtraction operation includes proper overflow checking before performing the operation
 */

#include <limits.h>
#include <stdio.h>

int safe_subtract(int a, int b, int *result) {
    if (b < 0 && a > INT_MAX + b) {
        return -1; // Positive overflow
    }
    if (b > 0 && a < INT_MIN + b) {
        return -1; // Negative overflow
    }
    *result = a - b;
    return 0;
}

int main() {
    int result;
    if (safe_subtract(INT_MIN + 10, 5, &result) == 0) {
        printf("Result: %d\n", result);
    } else {
        printf("Subtraction would overflow\n");
    }
    return 0;
}