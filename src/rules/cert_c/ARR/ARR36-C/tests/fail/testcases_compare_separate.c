/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Using relational operator on pointers to different arrays
 */

void compare_arrays(void) {
    int array1[5] = {1, 2, 3, 4, 5};
    int array2[5] = {6, 7, 8, 9, 10};
    int *ptr1 = &array1[2];
    int *ptr2 = &array2[3];

    // Compare pointers from different arrays using relational operator
    if (ptr1 < ptr2) {  // Line 14 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    compare_arrays();
    return 0;
}
