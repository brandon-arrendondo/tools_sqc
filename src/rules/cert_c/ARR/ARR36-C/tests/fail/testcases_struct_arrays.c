/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to arrays in different struct instances
 */

struct container {
    int data[10];
};

void struct_array_compare(void) {
    struct container c1;
    struct container c2;

    int *ptr1 = c1.data;
    int *ptr2 = c2.data;

    // Compare pointers from arrays in different structs
    if (ptr1 <= ptr2) {  // Line 19 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    struct_array_compare();
    return 0;
}
