/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <stdbool.h>

#define STACK_LIMIT 16384  // 16KB stack limit
#define MIN_ELEMENTS 1
#define MAX_ELEMENTS 4096

bool is_safe_vla_size(size_t elements, size_t element_size) {
    if (elements < MIN_ELEMENTS || elements > MAX_ELEMENTS) {
        return false;
    }

    if (element_size == 0) {
        return false;
    }

    // Check for overflow
    if (elements > SIZE_MAX / element_size) {
        return false;
    }

    size_t total_bytes = elements * element_size;
    return total_bytes <= STACK_LIMIT;
}

int create_int_array(size_t count) {
    if (!is_safe_vla_size(count, sizeof(int))) {
        printf("Unsafe VLA size for %zu integers\n", count);
        return -1;
    }

    int array[count];

    for (size_t i = 0; i < count; i++) {
        array[i] = i * i;
    }

    printf("Created safe int VLA with %zu elements\n", count);
    return 0;
}

int create_double_array(size_t count) {
    if (!is_safe_vla_size(count, sizeof(double))) {
        printf("Unsafe VLA size for %zu doubles\n", count);
        return -1;
    }

    double array[count];

    for (size_t i = 0; i < count; i++) {
        array[i] = i * 0.5;
    }

    printf("Created safe double VLA with %zu elements\n", count);
    return 0;
}

int create_char_buffer(size_t size) {
    if (!is_safe_vla_size(size, sizeof(char))) {
        printf("Unsafe buffer size: %zu bytes\n", size);
        return -1;
    }

    char buffer[size];

    for (size_t i = 0; i < size; i++) {
        buffer[i] = 'A' + (i % 26);
    }

    printf("Created safe char buffer of %zu bytes\n", size);
    return 0;
}

int main() {
    // Test various safe VLA sizes
    create_int_array(100);       // 400 bytes
    create_int_array(1000);      // 4000 bytes
    create_double_array(500);    // 4000 bytes
    create_double_array(1000);   // 8000 bytes
    create_char_buffer(8000);    // 8000 bytes
    create_char_buffer(16000);   // 16000 bytes

    // These should be rejected as unsafe
    create_int_array(10000);     // 40000 bytes - too large
    create_double_array(5000);   // 40000 bytes - too large

    return 0;
}