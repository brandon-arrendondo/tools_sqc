/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to different automatic arrays in same scope
 */

void auto_array_compare(void) {
    int auto1[12] = {0};
    int auto2[12] = {0};

    int *ptr1 = &auto1[4];
    int *ptr2 = &auto2[4];

    // Compare pointers from different automatic arrays
    if (ptr1 < ptr2) {  // Line 15 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    auto_array_compare();
    return 0;
}
