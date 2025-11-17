/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointer before and after realloc (different objects)
 */

#include <stdlib.h>

void realloc_compare(void) {
    int *original = (int *)malloc(10 * sizeof(int));
    int *old_ptr = original;

    if (original) {
        int *new_ptr = (int *)realloc(original, 20 * sizeof(int));

        if (new_ptr) {
            // Compare old and new pointers - may be different objects
            if (old_ptr < new_ptr) {  // Line 18 - VIOLATION
                // Undefined if realloc moved the memory
            }
            free(new_ptr);
        }
    }
}

int main(void) {
    realloc_compare();
    return 0;
}
