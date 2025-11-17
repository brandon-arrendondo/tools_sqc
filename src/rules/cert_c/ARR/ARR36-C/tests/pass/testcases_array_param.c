/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Pointer operations within same array parameter
 */

#include <stddef.h>
#include <stdio.h>

void process_array(int arr[], int size) {
    int *start = arr;
    int *end = arr + size;

    // Pointers within same array parameter - COMPLIANT
    ptrdiff_t length = end - start;
    printf("Array size: %td\n", length);

    int *middle = arr + (size / 2);
    if (start < middle && middle < end) {
        printf("Valid comparisons within array\n");
    }
}

int main(void) {
    int data[100] = {0};
    process_array(data, 100);
    return 0;
}
