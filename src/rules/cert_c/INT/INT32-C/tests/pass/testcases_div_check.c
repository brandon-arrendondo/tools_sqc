/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Division operation checks for the special case of INT_MIN / -1 which would overflow
 */

#include <limits.h>
#include <stdio.h>

int safe_divide(int dividend, int divisor, int *result) {
    if (divisor == 0) {
        return -2; // Division by zero
    }
    if (dividend == INT_MIN && divisor == -1) {
        return -1; // Would overflow
    }
    *result = dividend / divisor;
    return 0;
}

int main() {
    int result;
    int test_cases[][2] = {
        {100, 5},
        {-100, 5},
        {INT_MIN, -1},
        {INT_MIN, 2},
        {42, 0}
    };

    int count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < count; i++) {
        int ret = safe_divide(test_cases[i][0], test_cases[i][1], &result);
        if (ret == 0) {
            printf("%d / %d = %d\n", test_cases[i][0], test_cases[i][1], result);
        } else if (ret == -1) {
            printf("%d / %d would overflow\n", test_cases[i][0], test_cases[i][1]);
        } else {
            printf("%d / %d - division by zero\n", test_cases[i][0], test_cases[i][1]);
        }
    }

    return 0;
}