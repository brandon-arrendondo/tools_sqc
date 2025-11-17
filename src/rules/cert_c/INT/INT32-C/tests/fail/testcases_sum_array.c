/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Array summation can overflow without checking intermediate results
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int array[] = {INT_MAX / 2, INT_MAX / 2, INT_MAX / 2, 1000};
    int size = sizeof(array) / sizeof(array[0]);
    int sum = 0;

    // VIOLATION: no overflow checking in accumulation
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }

    printf("Sum: %d\n", sum);
    return 0;
}