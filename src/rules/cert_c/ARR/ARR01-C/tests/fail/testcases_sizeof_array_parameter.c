/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

/*
 * ARR01-C FAIL Case: Applying sizeof to array parameter
 *
 * This test case demonstrates a critical vulnerability where sizeof
 * is incorrectly applied to an array parameter. Array parameters
 * decay to pointers, so sizeof(array) returns the size of a pointer
 * (typically 4 or 8 bytes) rather than the array size.
 *
 * Vulnerability analysis:
 * - array parameter is actually int *array (pointer)
 * - sizeof(array) returns sizeof(int*) = 4 or 8 bytes
 * - sizeof(array[0]) returns sizeof(int) = 4 bytes
 * - Result: size = 1 or 2, not the actual array length of 10
 *
 * Security impact:
 * - Loop only processes first 1-2 elements instead of all 10
 * - Incomplete array processing
 * - If used for bounds checking, massive under-estimation
 * - Can lead to buffer overflows in other contexts
 *
 * Real-world consequences:
 * - Buffer overflow vulnerabilities
 * - Memory corruption
 * - Incomplete data processing
 * - Logic errors leading to security issues
 */

#include <stdio.h>

void clear_array(int array[]) {
    // VULNERABILITY: sizeof(array) returns pointer size, not array size
    // array parameter has decayed to int* pointer
    // On 64-bit systems: sizeof(array) = 8, sizeof(array[0]) = 4
    // Result: size = 2, not 10!
    size_t size = sizeof(array) / sizeof(array[0]);

    // This loop will only process 1-2 elements instead of all 10
    for (size_t i = 0; i < size; i++) {
        array[i] = 0;  // Only clears first 1-2 elements
    }

    printf("Incorrectly calculated size: %zu (should be 10)\n", size);
}

int main() {
    int numbers[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Array decays to pointer when passed to function
    clear_array(numbers);

    // Demonstrate that most elements remain unchanged
    printf("Array after 'clearing': ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    return 0;
}