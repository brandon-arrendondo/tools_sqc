/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof on nested struct member for pointer arithmetic
 */

#include <stddef.h>

struct inner {
    int data[5];
};

struct outer {
    struct inner in;
    int extra;
};

void nested_sizeof(void) {
    struct outer o;
    int *ptr = o.in.data;

    // Using sizeof for offset
    int *next = ptr + sizeof(struct inner);  // Line 23 - VIOLATION
    *next = 42;
}

int main(void) {
    nested_sizeof();
    return 0;
}
