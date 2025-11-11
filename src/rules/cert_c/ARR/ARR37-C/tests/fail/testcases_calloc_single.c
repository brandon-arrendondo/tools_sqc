/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single calloc'd object
 */

#include <stdlib.h>

void calloc_single(void) {
    // Allocate single int (treating 1 element as single object)
    int *ptr = (int *)calloc(1, sizeof(int));

    if (ptr) {
        *ptr = 42;
        // Pointer arithmetic beyond allocated object
        ptr[1] = 100;  // Line 16 - VIOLATION
        ptr[2] = 200;  // Line 17 - VIOLATION

        free(ptr);
    }
}

int main(void) {
    calloc_single();
    return 0;
}
