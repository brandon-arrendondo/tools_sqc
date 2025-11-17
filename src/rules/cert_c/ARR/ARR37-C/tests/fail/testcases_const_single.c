/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single const variable
 */

#include <stdio.h>

void const_test(void) {
    const int value = 777;
    const int *ptr = &value;

    // Pointer arithmetic on const single variable
    ptr = ptr + 1;  // Line 14 - VIOLATION
    printf("%d\n", *ptr);  // Undefined behavior
}

int main(void) {
    const_test();
    return 0;
}
