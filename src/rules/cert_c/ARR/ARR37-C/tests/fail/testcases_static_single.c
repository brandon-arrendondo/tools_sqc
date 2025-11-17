/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on static single variable
 */

void use_static(void) {
    static int counter = 0;
    int *ptr = &counter;

    // Increment pointer to static variable
    ptr++;  // Line 12 - VIOLATION
    *ptr = 10;  // Undefined behavior
}

int main(void) {
    use_static();
    return 0;
}
