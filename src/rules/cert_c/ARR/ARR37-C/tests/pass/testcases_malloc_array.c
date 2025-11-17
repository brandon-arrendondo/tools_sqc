/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Pointer arithmetic on dynamically allocated array
 */

#include <stdlib.h>
#include <stdio.h>

void malloc_array_test(void) {
    // Allocate array of 10 integers
    int *array = (int *)malloc(10 * sizeof(int));

    if (array) {
        int *ptr = array;

        // Initialize with pointer arithmetic - COMPLIANT
        for (int i = 0; i < 10; i++) {
            *(ptr + i) = i * 2;
        }

        // Access with array notation - COMPLIANT
        printf("array[5] = %d\n", array[5]);

        free(array);
    }
}

int main(void) {
    malloc_array_test();
    return 0;
}
