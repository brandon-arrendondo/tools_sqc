/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int allocate_and_process(size_t size) {
    int *buffer = malloc(size * sizeof(int));

    if (buffer == NULL) {
        printf("Allocation failed\n");
        free(buffer);  // Error: freeing NULL or invalid pointer
        return -1;
    }

    // Process data
    for (size_t i = 0; i < size; i++) {
        buffer[i] = i;
    }

    // Simulate error condition
    if (size > 1000) {
        printf("Size too large, cleaning up\n");
        free(buffer);
        return -1;
    }

    // Normal cleanup
    free(buffer);  // Double free if size > 1000

    return 0;
}

int main() {
    allocate_and_process(500);   // Normal case
    allocate_and_process(1500);  // Error case with double free

    return 0;
}