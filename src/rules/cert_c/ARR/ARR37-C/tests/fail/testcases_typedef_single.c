/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on typedef'd single variable
 */

typedef unsigned long ulong_t;

void typedef_test(void) {
    ulong_t value = 123456UL;
    ulong_t *ptr = &value;

    // Pointer arithmetic on single typedef'd variable
    ptr = ptr + 1;  // Line 14 - VIOLATION
    *ptr = 999UL;  // Undefined behavior
}

int main(void) {
    typedef_test();
    return 0;
}
