/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Pointer arithmetic on calloc'd array
 */

#include <stdlib.h>
#include <stdio.h>

void calloc_array_test(void) {
    // Allocate array of 15 doubles
    double *array = (double *)calloc(15, sizeof(double));

    if (array) {
        double *ptr = array;

        // Initialize with pointer arithmetic - COMPLIANT
        for (int i = 0; i < 15; i++) {
            *(ptr + i) = i * 1.5;
        }

        printf("array[10] = %.1f\n", array[10]);

        free(array);
    }
}

int main(void) {
    calloc_array_test();
    return 0;
}
