/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 8192

int validate_and_create_buffer(size_t base_size, size_t multiplier, size_t offset) {
    // Check for zero values
    if (base_size == 0 || multiplier == 0) {
        printf("Error: Base size and multiplier must be positive\n");
        return -1;
    }

    // Check for multiplication overflow
    if (base_size > SIZE_MAX / multiplier) {
        printf("Error: Multiplication would overflow\n");
        return -1;
    }

    size_t intermediate = base_size * multiplier;

    // Check for addition overflow
    if (intermediate > SIZE_MAX - offset) {
        printf("Error: Addition would overflow\n");
        return -1;
    }

    size_t final_size = intermediate + offset;

    // Check against reasonable limits
    if (final_size > MAX_BUFFER_SIZE) {
        printf("Error: Final size %zu exceeds limit %d\n", final_size, MAX_BUFFER_SIZE);
        return -1;
    }

    // Safe to create VLA
    char buffer[final_size];

    for (size_t i = 0; i < final_size; i++) {
        buffer[i] = (char)(i % 256);
    }

    printf("Successfully created buffer of calculated size: %zu\n", final_size);
    return 0;
}

int safe_power_of_two_array(unsigned int power) {
    if (power == 0) {
        printf("Error: Power must be positive\n");
        return -1;
    }

    if (power > 20) {  // 2^20 = 1MB, too large for stack
        printf("Error: 2^%u is too large for VLA\n", power);
        return -1;
    }

    size_t size = 1ULL << power;

    if (size > MAX_BUFFER_SIZE / sizeof(int)) {
        printf("Error: Array would be too large\n");
        return -1;
    }

    int array[size];

    for (size_t i = 0; i < size; i++) {
        array[i] = i;
    }

    printf("Created power-of-2 array: 2^%u = %zu elements\n", power, size);
    return 0;
}

int main() {
    // Safe size calculations
    validate_and_create_buffer(10, 20, 5);    // 205 bytes
    validate_and_create_buffer(50, 8, 0);     // 400 bytes
    validate_and_create_buffer(100, 4, 96);   // 496 bytes

    // Safe power-of-2 arrays
    safe_power_of_two_array(6);   // 64 elements
    safe_power_of_two_array(8);   // 256 elements
    safe_power_of_two_array(10);  // 1024 elements

    return 0;
}