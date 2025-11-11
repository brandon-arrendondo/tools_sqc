/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Function validates array size parameter before access
 */

#include <stdio.h>

void print_array(int *arr, size_t size) {
    // Validate parameters before array access
    if (arr == NULL || size == 0) {
        printf("Invalid array parameters\n");
        return;
    }

    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    int values[] = {100, 200, 300, 400};
    print_array(values, sizeof(values) / sizeof(values[0]));
    return 0;
}