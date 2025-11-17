/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to different extern arrays
 */

extern int extern_array1[15];
extern int extern_array2[15];

int extern_array1[15] = {0};
int extern_array2[15] = {0};

void compare_extern(void) {
    int *ptr1 = &extern_array1[5];
    int *ptr2 = &extern_array2[7];

    // Compare pointers from different extern arrays
    if (ptr1 <= ptr2) {  // Line 18 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    compare_extern();
    return 0;
}
