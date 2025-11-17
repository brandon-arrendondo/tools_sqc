/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Average calculation can overflow when summing before dividing
 */

#include <limits.h>
#include <stdio.h>

int calculate_average(int values[], int count) {
    int sum = 0;

    // VIOLATION: sum can overflow during accumulation
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }

    return sum / count;
}

int main() {
    int large_values[] = {
        INT_MAX / 2,
        INT_MAX / 2,
        INT_MAX / 3,
        INT_MAX / 4
    };

    int count = sizeof(large_values) / sizeof(large_values[0]);
    int avg = calculate_average(large_values, count);

    printf("Average: %d\n", avg);
    return 0;
}