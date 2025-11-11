/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Subtracting integer from pointer to single variable
 */

void subtract_test(void) {
    float num = 5.5f;
    float *ptr = &num;

    // Subtract from pointer to single variable
    float *prev = ptr - 1;  // Line 12 - VIOLATION
    *prev = 1.0f;  // Undefined behavior
}

int main(void) {
    subtract_test();
    return 0;
}
