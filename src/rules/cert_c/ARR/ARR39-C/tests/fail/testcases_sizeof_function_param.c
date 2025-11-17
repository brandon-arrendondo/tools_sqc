/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof on array parameter (decays to pointer)
 */

void process_array(int arr[]) {
    int *ptr = arr;

    // sizeof(arr) is sizeof(int*), but still wrong pattern
    int *end = ptr + sizeof(arr);  // Line 11 - VIOLATION
    *end = 100;
}

int main(void) {
    int data[50] = {0};
    process_array(data);
    return 0;
}
