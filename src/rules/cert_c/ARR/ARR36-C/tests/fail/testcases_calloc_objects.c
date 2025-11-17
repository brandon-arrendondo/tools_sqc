/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers from different calloc allocations
 */

#include <stdlib.h>
#include <stddef.h>

void calloc_subtract(void) {
    int *array1 = (int *)calloc(10, sizeof(int));
    int *array2 = (int *)calloc(10, sizeof(int));

    if (array1 && array2) {
        // Subtract pointers from different calloc objects
        ptrdiff_t diff = array2 - array1;  // Line 16 - VIOLATION

        free(array1);
        free(array2);
    }
}

int main(void) {
    calloc_subtract();
    return 0;
}
