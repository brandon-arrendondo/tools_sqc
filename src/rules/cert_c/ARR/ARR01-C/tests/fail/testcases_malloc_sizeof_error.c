/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void init_allocated_array(int *arr) {
    size_t count = sizeof(arr) / sizeof(int);

    for (size_t i = 0; i < count; i++) {
        arr[i] = i;
    }
}

int main() {
    int *numbers = malloc(50 * sizeof(int));
    if (numbers) {
        init_allocated_array(numbers);
        free(numbers);
    }

    return 0;
}