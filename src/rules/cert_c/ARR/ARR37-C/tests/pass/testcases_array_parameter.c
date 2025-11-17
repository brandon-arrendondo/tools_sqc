/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Pointer arithmetic on array passed as parameter
 */

#include <stdio.h>

void process_array(int arr[], int size) {
    int *ptr = arr;

    // Pointer arithmetic on array parameter - COMPLIANT
    for (int i = 0; i < size; i++) {
        *(ptr + i) = i * 10;
    }

    // Array subscript notation - COMPLIANT
    printf("arr[3] = %d\n", arr[3]);
}

int main(void) {
    int data[20] = {0};
    process_array(data, 20);
    return 0;
}
