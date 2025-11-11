/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_POINTERS 5

int main() {
    int *pointers[MAX_POINTERS] = {NULL};
    int allocated_count = 0;

    // Allocate multiple memory blocks
    for (int i = 0; i < MAX_POINTERS; i++) {
        pointers[i] = malloc((i + 1) * 10 * sizeof(int));
        if (pointers[i]) {
            allocated_count++;
            printf("Allocated block %d: %d integers\n", i, (i + 1) * 10);

            // Initialize the memory
            for (int j = 0; j < (i + 1) * 10; j++) {
                pointers[i][j] = j;
            }
        } else {
            printf("Allocation %d failed\n", i);
        }
    }

    printf("Successfully allocated %d blocks\n", allocated_count);

    // Free all allocated memory exactly once
    for (int i = 0; i < MAX_POINTERS; i++) {
        if (pointers[i] != NULL) {
            free(pointers[i]);
            pointers[i] = NULL;  // Prevent accidental reuse
            printf("Freed block %d\n", i);
        }
    }

    printf("All memory freed successfully\n");
    return 0;
}