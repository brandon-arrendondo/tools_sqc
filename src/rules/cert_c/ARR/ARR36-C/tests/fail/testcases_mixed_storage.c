/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to arrays with different storage duration
 */

static int static_array[10] = {0};

void mixed_compare(void) {
    int auto_array[10] = {0};

    int *ptr1 = static_array;
    int *ptr2 = auto_array;

    // Compare static and automatic array pointers
    if (ptr1 > ptr2) {  // Line 16 - VIOLATION
        // Undefined behavior - different storage classes
    }
}

int main(void) {
    mixed_compare();
    return 0;
}
