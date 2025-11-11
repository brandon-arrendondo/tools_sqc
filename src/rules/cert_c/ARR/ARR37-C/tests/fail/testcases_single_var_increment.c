/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Incrementing pointer to a single variable
 */

#include <stdio.h>

void increment_single(void) {
    int value = 42;
    int *ptr = &value;

    // Increment pointer to single variable and dereference
    ptr++;  // Line 13 - VIOLATION (ptr + 1 is valid, but dereferencing is UB)
    printf("%d\n", *ptr);  // Line 14 - Undefined behavior
}

int main(void) {
    increment_single();
    return 0;
}
