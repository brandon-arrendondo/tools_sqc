/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Negation operation checks for INT_MIN before negating to avoid overflow
 */

#include <limits.h>
#include <stdio.h>

int safe_negate(int a, int *result) {
    if (a == INT_MIN) {
        return -1; // Negating INT_MIN would overflow
    }
    *result = -a;
    return 0;
}

int main() {
    int result;
    int value = -42;

    if (safe_negate(value, &result) == 0) {
        printf("Negation of %d is %d\n", value, result);
    } else {
        printf("Negation would overflow\n");
    }

    if (safe_negate(INT_MIN, &result) == 0) {
        printf("Result: %d\n", result);
    } else {
        printf("Cannot negate INT_MIN - would overflow\n");
    }

    return 0;
}