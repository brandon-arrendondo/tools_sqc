/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Multiplication operation includes proper overflow checking using division method
 */

#include <limits.h>
#include <stdio.h>

int safe_multiply(int a, int b, int *result) {
    if (a == 0 || b == 0) {
        *result = 0;
        return 0;
    }

    if (a > 0) {
        if (b > 0) {
            if (a > INT_MAX / b) return -1; // Positive overflow
        } else {
            if (b < INT_MIN / a) return -1; // Negative overflow
        }
    } else {
        if (b > 0) {
            if (a < INT_MIN / b) return -1; // Negative overflow
        } else {
            if (a != -1 && b < INT_MAX / a) return -1; // Positive overflow
        }
    }

    *result = a * b;
    return 0;
}

int main() {
    int result;
    if (safe_multiply(1000, 2000, &result) == 0) {
        printf("Result: %d\n", result);
    } else {
        printf("Multiplication would overflow\n");
    }
    return 0;
}