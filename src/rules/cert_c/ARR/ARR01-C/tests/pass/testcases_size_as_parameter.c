/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

/*
 * ARR01-C PASS Case: Correct array size handling
 *
 * This test case demonstrates the proper way to handle array sizes
 * when passing arrays to functions. The key principle is calculating
 * the array size where the array is declared, then passing both
 * the array and its size to functions.
 *
 * Compliant practices:
 * - Calculate size at declaration site using sizeof
 * - Pass size as separate parameter to functions
 * - Use size_t for array indices and sizes
 * - Avoid applying sizeof to array parameters
 *
 * Security benefits:
 * - Prevents buffer overflow from size miscalculation
 * - Ensures proper bounds checking in functions
 * - Maintains correct array bounds information
 */

#include <stdio.h>

// Function receives array pointer and explicit size parameter
// Note: Cannot use sizeof(arr) here as arr is a pointer parameter
void process_array(int arr[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        arr[i] = arr[i] * 2;
    }
}

int main() {
    int numbers[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // CORRECT: Calculate array size where array is declared
    // sizeof(numbers) gives total bytes, sizeof(numbers[0]) gives element size
    size_t array_size = sizeof(numbers) / sizeof(numbers[0]);

    // Pass both array and its calculated size
    process_array(numbers, array_size);

    for (size_t i = 0; i < array_size; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    return 0;
}