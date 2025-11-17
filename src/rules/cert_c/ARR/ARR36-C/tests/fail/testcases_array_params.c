/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing array parameters that point to different arrays
 */

void process_arrays(int arr1[], int arr2[], int size) {
    int *ptr1 = arr1;
    int *ptr2 = arr2;

    // Compare pointers from different array parameters
    if (ptr1 >= ptr2) {  // Line 12 - VIOLATION
        // Undefined behavior - arr1 and arr2 are different arrays
    }
}

int main(void) {
    int a[10] = {0};
    int b[10] = {0};
    process_arrays(a, b, 10);
    return 0;
}
