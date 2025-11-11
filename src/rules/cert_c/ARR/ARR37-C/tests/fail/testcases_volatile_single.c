/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single volatile variable
 */

void volatile_test(void) {
    volatile int sensor = 0;
    volatile int *ptr = &sensor;

    // Pointer arithmetic on volatile single variable
    ptr++;  // Line 12 - VIOLATION
    *ptr = 1;  // Undefined behavior
}

int main(void) {
    volatile_test();
    return 0;
}
