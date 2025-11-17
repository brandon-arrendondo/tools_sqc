/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <errno.h>

#define MIN_SIZE 1
#define MAX_SIZE 2048

typedef enum {
    VLA_SUCCESS = 0,
    VLA_ERROR_ZERO_SIZE,
    VLA_ERROR_TOO_LARGE,
    VLA_ERROR_INVALID_INPUT
} vla_result_t;

vla_result_t create_and_fill_array(size_t size, int fill_value) {
    if (size == 0) {
        printf("Error: Array size cannot be zero\n");
        return VLA_ERROR_ZERO_SIZE;
    }

    if (size > MAX_SIZE) {
        printf("Error: Array size %zu exceeds maximum %d\n", size, MAX_SIZE);
        return VLA_ERROR_TOO_LARGE;
    }

    int array[size];

    for (size_t i = 0; i < size; i++) {
        array[i] = fill_value;
    }

    printf("Successfully created and filled array of size %zu\n", size);
    return VLA_SUCCESS;
}

int main() {
    vla_result_t result;

    result = create_and_fill_array(10, 42);
    if (result != VLA_SUCCESS) {
        printf("Failed to create array\n");
    }

    result = create_and_fill_array(100, 0);
    if (result != VLA_SUCCESS) {
        printf("Failed to create array\n");
    }

    result = create_and_fill_array(0, 1);
    if (result == VLA_ERROR_ZERO_SIZE) {
        printf("Properly handled zero size error\n");
    }

    return 0;
}