/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    // Initial allocation
    int *array = malloc(5 * sizeof(int));
    if (!array) {
        printf("Initial allocation failed\n");
        return 1;
    }

    // Initialize array
    for (int i = 0; i < 5; i++) {
        array[i] = i;
    }

    printf("Initial array size: 5\n");

    // Safe realloc usage
    int *temp = realloc(array, 10 * sizeof(int));
    if (temp) {
        // Realloc succeeded
        array = temp;

        // Initialize new elements
        for (int i = 5; i < 10; i++) {
            array[i] = i;
        }

        printf("Array successfully resized to 10 elements\n");
    } else {
        // Realloc failed, original array is still valid
        printf("Realloc failed, keeping original array\n");
    }

    // Free the memory exactly once
    free(array);
    array = NULL;

    printf("Memory freed successfully\n");
    return 0;
}