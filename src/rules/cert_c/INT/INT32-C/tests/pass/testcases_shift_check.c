/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Left shift operation checks for valid shift count and potential overflow
 */

#include <limits.h>
#include <stdio.h>
#include <stdint.h>

int safe_left_shift(int value, int shift_count, int *result) {
    if (shift_count < 0 || shift_count >= 32) {
        return -1; // Invalid shift count
    }

    if (value > 0 && value > (INT_MAX >> shift_count)) {
        return -1; // Would overflow
    }

    if (value < 0) {
        return -1; // Left shifting negative values is undefined behavior
    }

    *result = value << shift_count;
    return 0;
}

int main() {
    int result;
    int test_cases[][2] = {
        {1, 30},
        {1000, 10},
        {INT_MAX / 4, 1},
        {-5, 2},
        {1, 35}
    };

    int count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < count; i++) {
        if (safe_left_shift(test_cases[i][0], test_cases[i][1], &result) == 0) {
            printf("%d << %d = %d\n", test_cases[i][0], test_cases[i][1], result);
        } else {
            printf("%d << %d would overflow or is invalid\n", test_cases[i][0], test_cases[i][1]);
        }
    }

    return 0;
}