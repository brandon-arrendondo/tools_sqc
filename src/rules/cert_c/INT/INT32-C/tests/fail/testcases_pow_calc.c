/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Power calculation using repeated multiplication can overflow
 */

#include <limits.h>
#include <stdio.h>

int power(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base; // VIOLATION: no overflow checking
    }
    return result;
}

int main() {
    int test_cases[][2] = {
        {2, 30},
        {10, 9},
        {-2, 31},
        {100, 5}
    };

    int count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < count; i++) {
        int result = power(test_cases[i][0], test_cases[i][1]);
        printf("%d^%d = %d\n", test_cases[i][0], test_cases[i][1], result);
    }

    return 0;
}