/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers from arrays in nested structures
 */

#include <stddef.h>

struct inner {
    int arr[5];
};

struct outer {
    struct inner in1;
    struct inner in2;
};

void nested_subtract(void) {
    struct outer o;

    int *ptr1 = o.in1.arr;
    int *ptr2 = o.in2.arr;

    // Subtract pointers from different nested struct arrays
    ptrdiff_t diff = ptr2 - ptr1;  // Line 26 - VIOLATION
}

int main(void) {
    nested_subtract();
    return 0;
}
