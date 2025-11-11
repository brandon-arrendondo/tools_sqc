/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Array is allocated, used, and properly freed with null check
 */

#include <stdlib.h>

int *create_array(int count) {
    int *arr = malloc(count * sizeof(int));
    if (arr == NULL) {
        return NULL;
    }

    // Initialize array
    for (int i = 0; i < count; i++) {
        arr[i] = i * 2;
    }

    return arr;
}

void use_array() {
    int *numbers = create_array(10);
    if (numbers != NULL) {
        // Use the array
        int sum = 0;
        for (int i = 0; i < 10; i++) {
            sum += numbers[i];
        }

        // Properly free the memory
        free(numbers);
    }
}