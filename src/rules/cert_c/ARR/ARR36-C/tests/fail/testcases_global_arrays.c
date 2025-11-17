/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers from different global arrays
 */

int global1[20];
int global2[20];

void compare_globals(void) {
    int *ptr1 = &global1[5];
    int *ptr2 = &global2[5];

    // Compare pointers from different global arrays
    if (ptr1 <= ptr2) {  // Line 15 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    compare_globals();
    return 0;
}
