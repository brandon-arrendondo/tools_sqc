/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single aligned_alloc'd object
 */

#include <stdlib.h>

void aligned_single(void) {
    int *ptr = (int *)aligned_alloc(16, sizeof(int));

    if (ptr) {
        *ptr = 42;
        // Pointer arithmetic on single aligned allocation
        *(ptr + 1) = 100;  // Line 15 - VIOLATION

        free(ptr);
    }
}

int main(void) {
    aligned_single();
    return 0;
}
