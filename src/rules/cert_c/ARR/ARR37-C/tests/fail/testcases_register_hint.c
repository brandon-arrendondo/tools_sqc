/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on register-hinted variable (if addressable)
 */

#include <stdio.h>

void register_test(void) {
    register int value = 100;  // May not actually be in register
    int *ptr = &value;  // Taking address forces to memory

    // Pointer arithmetic on single variable
    ptr++;  // Line 13 - VIOLATION
    printf("%d\n", *ptr);  // Undefined behavior
}

int main(void) {
    register_test();
    return 0;
}
