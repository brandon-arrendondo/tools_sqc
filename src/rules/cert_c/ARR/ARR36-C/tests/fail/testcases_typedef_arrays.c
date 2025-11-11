/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers to different arrays despite same typedef
 */

#include <stddef.h>

typedef int int_array[10];

void typedef_subtract(void) {
    int_array arr1;
    int_array arr2;

    int *ptr1 = arr1;
    int *ptr2 = arr2;

    // Subtract pointers from different arrays (same type doesn't matter)
    ptrdiff_t diff = ptr2 - ptr1;  // Line 19 - VIOLATION
}

int main(void) {
    typedef_subtract();
    return 0;
}
