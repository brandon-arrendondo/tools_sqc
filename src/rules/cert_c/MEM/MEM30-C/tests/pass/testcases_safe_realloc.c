/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Old pointer is not accessed after realloc, new pointer is used instead
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(5 * sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    // Initialize array
    for (int i = 0; i < 5; i++) {
        ptr[i] = i + 1;
    }

    // Safely reallocate
    int *new_ptr = realloc(ptr, 10 * sizeof(int));
    if (new_ptr == NULL) {
        free(ptr);
        return -1;
    }

    ptr = new_ptr;  // Update pointer to new memory

    // Access through new pointer only
    for (int i = 5; i < 10; i++) {
        ptr[i] = i + 1;
    }

    free(ptr);
    ptr = NULL;
    return 0;
}