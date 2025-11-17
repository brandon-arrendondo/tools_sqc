/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Dynamically allocated array is never freed
 */

#include <stdlib.h>

int *create_array(int size) {
    int *arr = malloc(size * sizeof(int));
    if (arr == NULL) {
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        arr[i] = i * i;
    }

    return arr;
}

void use_array() {
    int *numbers = create_array(50);
    if (numbers != NULL) {
        int sum = 0;
        for (int i = 0; i < 50; i++) {
            sum += numbers[i];
        }
        printf("Sum: %d\n", sum);
    }
    // Array is never freed - MEMORY LEAK
}