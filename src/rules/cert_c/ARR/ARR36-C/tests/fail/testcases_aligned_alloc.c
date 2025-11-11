/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers from different aligned_alloc allocations
 */

#include <stdlib.h>
#include <stddef.h>

void aligned_subtract(void) {
    int *array1 = (int *)aligned_alloc(16, 64);
    int *array2 = (int *)aligned_alloc(16, 64);

    if (array1 && array2) {
        // Subtract pointers from different aligned allocations
        ptrdiff_t diff = array2 - array1;  // Line 16 - VIOLATION

        free(array1);
        free(array2);
    }
}

int main(void) {
    aligned_subtract();
    return 0;
}
