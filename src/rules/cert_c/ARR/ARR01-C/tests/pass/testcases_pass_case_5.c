/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>

void process_array(int arr[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        arr[i] = arr[i] * 2;
    }
}

int main() {
    int numbers[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t array_size = sizeof(numbers) / sizeof(numbers[0]);
    
    process_array(numbers, array_size);
    return 0;
}
