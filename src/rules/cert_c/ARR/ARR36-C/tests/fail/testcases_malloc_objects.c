/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers from different dynamically allocated objects
 */

#include <stdlib.h>
#include <stddef.h>

void dynamic_alloc(void) {
    int *array1 = (int *)malloc(10 * sizeof(int));
    int *array2 = (int *)malloc(10 * sizeof(int));

    if (array1 && array2) {
        // Subtract pointers from different allocated objects
        ptrdiff_t diff = array2 - array1;  // Line 16 - VIOLATION

        free(array1);
        free(array2);
    }
}

int main(void) {
    dynamic_alloc();
    return 0;
}
