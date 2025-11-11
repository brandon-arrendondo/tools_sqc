/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Proper bounds checking before array access prevents out-of-bounds access
 */

#include <stdio.h>
#include <stddef.h>

#define ARRAY_SIZE 10

int safe_array_access(int arr[], size_t size, size_t index) {
    if (index < size) {
        return arr[index];
    }
    return -1; // Error value
}

int main(void) {
    int numbers[ARRAY_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        printf("Element %zu: %d\n", i, safe_array_access(numbers, ARRAY_SIZE, i));
    }

    return 0;
}