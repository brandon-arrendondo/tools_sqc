/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to different sub-arrays in array of arrays
 */

void array_of_arrays(void) {
    int arrays[3][5];

    int *ptr1 = arrays[0];  // Points to first sub-array
    int *ptr2 = arrays[1];  // Points to second sub-array

    // While both are in same memory block, they are different arrays
    if (ptr1 < ptr2) {  // Line 14 - VIOLATION (conceptually different arrays)
        // This comparison is technically undefined per strict interpretation
    }
}

int main(void) {
    array_of_arrays();
    return 0;
}
