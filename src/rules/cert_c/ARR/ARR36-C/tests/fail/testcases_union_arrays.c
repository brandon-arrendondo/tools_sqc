/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers to different members of union
 */

#include <stddef.h>

union mixed {
    int int_array[10];
    float float_array[10];
};

void union_subtract(void) {
    union mixed u;
    int *ptr1 = u.int_array;
    float *ptr2 = u.float_array;

    // Different types, but even with casting, different array objects
    ptrdiff_t diff = (int *)ptr2 - ptr1;  // Line 20 - VIOLATION
}

int main(void) {
    union_subtract();
    return 0;
}
