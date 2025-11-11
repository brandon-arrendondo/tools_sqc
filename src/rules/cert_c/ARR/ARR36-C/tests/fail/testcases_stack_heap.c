/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointer to stack array with pointer to heap array
 */

#include <stdlib.h>

void stack_vs_heap(void) {
    int stack_array[10] = {0};
    int *heap_array = (int *)malloc(10 * sizeof(int));

    if (heap_array) {
        int *ptr1 = &stack_array[3];
        int *ptr2 = &heap_array[3];

        // Compare stack and heap pointers
        if (ptr1 > ptr2) {  // Line 18 - VIOLATION
            // Undefined behavior
        }

        free(heap_array);
    }
}

int main(void) {
    stack_vs_heap();
    return 0;
}
