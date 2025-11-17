/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Range calculation between two points can overflow on subtraction
 */

#include <limits.h>
#include <stdio.h>

int calculate_range(int start, int end) {
    // VIOLATION: subtraction can overflow
    return end - start;
}

int main() {
    int test_cases[][2] = {
        {INT_MIN, INT_MAX},     // Maximum possible range
        {-1000000, INT_MAX},    // Large positive range
        {INT_MAX, -1000000},    // Large negative range (end - start)
        {INT_MIN, 1000000}      // Another problematic case
    };

    int count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < count; i++) {
        int range = calculate_range(test_cases[i][0], test_cases[i][1]);
        printf("Range from %d to %d: %d\n",
               test_cases[i][0], test_cases[i][1], range);
    }

    return 0;
}