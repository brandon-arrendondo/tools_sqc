/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int allocate_and_process(size_t size) {
    int *buffer = malloc(size * sizeof(int));

    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return -1;  // No free() needed for NULL pointer
    }

    // Process the allocated memory
    for (size_t i = 0; i < size; i++) {
        buffer[i] = i + 1;
    }

    printf("Processed %zu elements\n", size);

    // Free exactly once before returning
    free(buffer);
    buffer = NULL;

    return 0;
}

int main() {
    // Test with valid allocation
    if (allocate_and_process(100) == 0) {
        printf("Successfully processed data\n");
    }

    // Test with potentially failing allocation
    if (allocate_and_process(1000000000) != 0) {
        printf("Large allocation handled gracefully\n");
    }

    return 0;
}