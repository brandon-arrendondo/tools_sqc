/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Pointer arithmetic within same dynamically allocated array
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

void dynamic_array(void) {
    int *array = (int *)malloc(50 * sizeof(int));

    if (array) {
        int *ptr1 = &array[10];
        int *ptr2 = &array[40];

        // Subtract pointers within same dynamic array - COMPLIANT
        ptrdiff_t diff = ptr2 - ptr1;
        printf("Distance: %td\n", diff);

        // Compare within same array - COMPLIANT
        if (ptr1 <= ptr2) {
            printf("ptr1 is before or at ptr2\n");
        }

        free(array);
    }
}

int main(void) {
    dynamic_array();
    return 0;
}
