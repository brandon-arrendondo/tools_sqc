/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on restrict-qualified single parameter
 */

void restrict_test(int *restrict ptr) {
    // Assume ptr points to single int, not array
    *ptr = 10;

    // Pointer arithmetic on single restrict pointer
    *(ptr + 1) = 20;  // Line 12 - VIOLATION
    ptr[2] = 30;  // Line 13 - VIOLATION
}

int main(void) {
    int value = 0;
    restrict_test(&value);
    return 0;
}
