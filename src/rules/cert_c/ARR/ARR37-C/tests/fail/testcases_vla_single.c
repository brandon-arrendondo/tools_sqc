/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Treating pointer to single VLA element as multi-element array
 */

void vla_test(int n) {
    int vla[n];
    vla[0] = 100;

    // Get pointer to single element
    int *ptr = &vla[0];

    // Incorrect assumption: treating element pointer as if it were array base
    // in context where we lose track that it's part of larger array
    ptr += n;  // Line 16 - VIOLATION (if ptr treated as single-element pointer)
    *ptr = 200;  // May be out of bounds
}

int main(void) {
    vla_test(1);  // With n=1, ptr+n is definitely out of bounds
    return 0;
}
