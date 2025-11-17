/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to different const arrays
 */

const int const_array1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
const int const_array2[8] = {9, 10, 11, 12, 13, 14, 15, 16};

void compare_const(void) {
    const int *ptr1 = &const_array1[2];
    const int *ptr2 = &const_array2[3];

    // Compare pointers from different const arrays
    if (ptr1 > ptr2) {  // Line 15 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    compare_const();
    return 0;
}
