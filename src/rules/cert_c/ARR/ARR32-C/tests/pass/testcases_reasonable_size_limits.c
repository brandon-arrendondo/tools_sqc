/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <stdlib.h>

#define SMALL_ARRAY_LIMIT 100
#define MEDIUM_ARRAY_LIMIT 1000
#define LARGE_ARRAY_THRESHOLD 10000

void process_small_array(size_t n) {
    if (n == 0 || n > SMALL_ARRAY_LIMIT) {
        printf("Invalid size for small array: %zu\n", n);
        return;
    }

    int small_array[n];

    for (size_t i = 0; i < n; i++) {
        small_array[i] = i;
    }

    printf("Processed small VLA of size %zu\n", n);
}

void process_medium_array(size_t n) {
    if (n == 0 || n > MEDIUM_ARRAY_LIMIT) {
        printf("Invalid size for medium array: %zu\n", n);
        return;
    }

    double medium_array[n];

    for (size_t i = 0; i < n; i++) {
        medium_array[i] = i * 0.5;
    }

    printf("Processed medium VLA of size %zu\n", n);
}

void process_large_data(size_t n) {
    if (n == 0) {
        printf("Size must be positive\n");
        return;
    }

    if (n > LARGE_ARRAY_THRESHOLD) {
        printf("Using dynamic allocation for large size: %zu\n", n);
        int *large_array = malloc(n * sizeof(int));
        if (large_array) {
            // Process with malloc'd array
            free(large_array);
        }
        return;
    }

    int stack_array[n];
    printf("Using stack VLA for size: %zu\n", n);
}

int main() {
    process_small_array(50);
    process_medium_array(500);
    process_large_data(100);
    process_large_data(20000);  // Uses malloc instead

    return 0;
}