/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to different function parameters
 */

void process(int *a, int *b, int *c) {
    // Compare pointers to different parameters
    if (a < b) {  // Line 10 - VIOLATION
        // Undefined behavior - a and b are not from same array
    }

    if (b >= c) {  // Line 14 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    int x = 1, y = 2, z = 3;
    process(&x, &y, &z);
    return 0;
}
