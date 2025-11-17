/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Adding integer to pointer to single variable and dereferencing
 */

#include <stdio.h>

void add_to_single(void) {
    int x = 100;
    int *ptr = &x;

    // Add integer to pointer to single variable
    int *new_ptr = ptr + 2;  // Line 14 - VIOLATION
    printf("%d\n", *new_ptr);  // Undefined behavior
}

int main(void) {
    add_to_single();
    return 0;
}
