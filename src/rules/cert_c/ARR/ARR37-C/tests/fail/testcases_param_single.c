/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on pointer to single parameter
 */

void modify_param(int *value) {
    // Treat parameter pointer as if it were an array
    value[1] = 100;  // Line 9 - VIOLATION (*(value + 1))
    *(value + 2) = 200;  // Line 10 - VIOLATION
}

int main(void) {
    int x = 42;
    modify_param(&x);
    return 0;
}
