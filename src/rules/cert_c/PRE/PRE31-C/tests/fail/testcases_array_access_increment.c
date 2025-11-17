/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Array access with increment in unsafe macro
 */

#define DOUBLE(x) ((x) + (x))  /* UNSAFE */

void array_operation(int arr[], int idx) {
    // arr[idx++] has side effect - idx incremented twice
    int result = DOUBLE(arr[idx++]);  // Line 11 - VIOLATION
}

int main(void) {
    int data[] = {1, 2, 3, 4, 5};
    array_operation(data, 0);
    return 0;
}
