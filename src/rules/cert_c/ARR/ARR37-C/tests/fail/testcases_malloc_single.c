/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single malloc'd object
 */

#include <stdlib.h>

void malloc_single(void) {
    int *ptr = (int *)malloc(sizeof(int));

    if (ptr) {
        *ptr = 42;
        // Treat single malloc'd int as array
        ptr[1] = 100;  // Line 15 - VIOLATION
        *(ptr + 2) = 200;  // Line 16 - VIOLATION

        free(ptr);
    }
}

int main(void) {
    malloc_single();
    return 0;
}
