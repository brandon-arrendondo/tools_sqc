/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Array index calculation checks for overflow before computing the final index
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int safe_array_index(int base_index, int offset, int array_size, int *final_index) {
    // Check for addition overflow
    if (offset > 0 && base_index > INT_MAX - offset) {
        return -1; // Addition would overflow
    }
    if (offset < 0 && base_index < INT_MIN - offset) {
        return -1; // Subtraction would underflow
    }

    int index = base_index + offset;

    // Check bounds
    if (index < 0 || index >= array_size) {
        return -2; // Out of bounds
    }

    *final_index = index;
    return 0;
}

int main() {
    int array[100];
    int final_index;

    // Initialize array
    for (int i = 0; i < 100; i++) {
        array[i] = i * i;
    }

    int test_cases[][3] = {
        {10, 5, 100},      // base=10, offset=5, size=100
        {50, -10, 100},    // base=50, offset=-10, size=100
        {INT_MAX - 10, 5, 100},  // Would overflow
        {90, 20, 100}      // Out of bounds
    };

    int count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < count; i++) {
        int ret = safe_array_index(test_cases[i][0], test_cases[i][1], test_cases[i][2], &final_index);
        if (ret == 0) {
            printf("array[%d + %d] = array[%d] = %d\n",
                   test_cases[i][0], test_cases[i][1], final_index, array[final_index]);
        } else if (ret == -1) {
            printf("Index calculation %d + %d would overflow\n", test_cases[i][0], test_cases[i][1]);
        } else {
            printf("Index %d + %d is out of bounds\n", test_cases[i][0], test_cases[i][1]);
        }
    }

    return 0;
}