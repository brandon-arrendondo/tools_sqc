/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single global variable
 */

int global_var = 500;

void access_global(void) {
    int *ptr = &global_var;

    // Pointer arithmetic on single global variable
    int *next = ptr + 1;  // Line 13 - VIOLATION
    *next = 600;  // Undefined behavior
}

int main(void) {
    access_global();
    return 0;
}
