/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to different compound literals
 */

void compound_lits(void) {
    int *ptr1 = (int[]){1, 2, 3, 4, 5};
    int *ptr2 = (int[]){6, 7, 8, 9, 10};

    // Compare pointers from different compound literals
    if (ptr1 < ptr2) {  // Line 12 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    compound_lits();
    return 0;
}
