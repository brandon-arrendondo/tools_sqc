/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Decrementing pointer to single variable
 */

void decrement_test(void) {
    double value = 3.14;
    double *ptr = &value;

    // Decrement pointer to single variable
    ptr--;  // Line 12 - VIOLATION
    *ptr = 2.71;  // Undefined behavior
}

int main(void) {
    decrement_test();
    return 0;
}
