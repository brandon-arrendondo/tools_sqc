/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single pointer-to-pointer
 */

void pointer_to_pointer_test(void) {
    int value = 42;
    int *ptr1 = &value;
    int **ptr2 = &ptr1;

    // Pointer arithmetic on single pointer-to-pointer
    ptr2++;  // Line 13 - VIOLATION
    *ptr2 = NULL;  // Undefined behavior
}

int main(void) {
    pointer_to_pointer_test();
    return 0;
}
