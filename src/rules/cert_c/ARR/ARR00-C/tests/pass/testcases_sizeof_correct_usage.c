/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void process_array(int arr[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        arr[i] = arr[i] * 2;
    }
}

int main() {
    int static_array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    size_t array_size = sizeof(static_array) / sizeof(static_array[0]);
    printf("Array has %zu elements\n", array_size);

    size_t array_bytes = sizeof(static_array);
    printf("Array occupies %zu bytes\n", array_bytes);

    process_array(static_array, array_size);

    int *dynamic_array = malloc(5 * sizeof(int));
    if (dynamic_array != NULL) {
        for (int i = 0; i < 5; i++) {
            dynamic_array[i] = i;
        }
        free(dynamic_array);
    }

    return 0;
}