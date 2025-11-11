/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers between struct member and separate array
 */

#include <stddef.h>

struct data {
    int buffer[10];
    int value;
};

void mixed_subtract(void) {
    struct data d;
    int separate[10];

    int *ptr1 = d.buffer;
    int *ptr2 = separate;

    // Subtract pointer from struct member array and separate array
    ptrdiff_t diff = ptr2 - ptr1;  // Line 23 - VIOLATION
}

int main(void) {
    mixed_subtract();
    return 0;
}
