/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <stdint.h>

#define MAX_ELEMENTS 1000

int process_buffer(size_t element_count, size_t element_size) {
    if (element_count == 0 || element_size == 0) {
        printf("Error: Both element_count and element_size must be positive\n");
        return -1;
    }

    if (element_count > MAX_ELEMENTS) {
        printf("Error: Too many elements requested\n");
        return -1;
    }

    if (element_size > SIZE_MAX / element_count) {
        printf("Error: Total size would overflow\n");
        return -1;
    }

    size_t total_size = element_count * element_size;
    if (total_size > 8192) {  // Reasonable stack limit
        printf("Error: Total buffer size too large for stack allocation\n");
        return -1;
    }

    char buffer[total_size];

    for (size_t i = 0; i < total_size; i++) {
        buffer[i] = (char)(i % 256);
    }

    printf("Successfully created buffer of %zu bytes\n", total_size);
    return 0;
}

int main() {
    process_buffer(100, 4);   // 400 bytes
    process_buffer(50, 8);    // 400 bytes
    process_buffer(10, 32);   // 320 bytes

    return 0;
}