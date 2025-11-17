/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof union for pointer offset
 */

#include <stddef.h>

union data {
    int i;
    float f;
    char c[8];
};

void union_sizeof_offset(void) {
    union data array[20];
    union data *ptr = array;

    // Using sizeof(union data) as offset
    union data *next = ptr + sizeof(union data);  // Line 20 - VIOLATION
    next->i = 42;
}

int main(void) {
    union_sizeof_offset();
    return 0;
}
