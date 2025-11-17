/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: PASS
 * Reason: Memory size calculation checks for multiplication overflow before allocating
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int safe_size_calculation(int num_elements, int element_size, int *total_size) {
    if (num_elements <= 0 || element_size <= 0) {
        return -1; // Invalid parameters
    }

    // Check for multiplication overflow
    if (num_elements > INT_MAX / element_size) {
        return -1; // Multiplication would overflow
    }

    *total_size = num_elements * element_size;
    return 0;
}

void* safe_malloc(int num_elements, int element_size) {
    int total_size;
    if (safe_size_calculation(num_elements, element_size, &total_size) != 0) {
        return NULL; // Size calculation failed
    }
    return malloc(total_size);
}

int main() {
    int test_cases[][2] = {
        {100, sizeof(int)},
        {1000, sizeof(double)},
        {INT_MAX / 2, 4},
        {INT_MAX, sizeof(int)}  // Would overflow
    };

    int count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < count; i++) {
        void* ptr = safe_malloc(test_cases[i][0], test_cases[i][1]);
        if (ptr != NULL) {
            printf("Successfully allocated %d elements of size %d\n",
                   test_cases[i][0], test_cases[i][1]);
            free(ptr);
        } else {
            printf("Failed to allocate %d elements of size %d (would overflow)\n",
                   test_cases[i][0], test_cases[i][1]);
        }
    }

    return 0;
}