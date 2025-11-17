/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int *ptr = arr;
    int *end = arr + 10;

    while (ptr < end) {
        *ptr = *ptr * 2;
        ptr++;
    }

    ptr = &arr[5];
    if (ptr >= arr && ptr < arr + 10) {
        *ptr = 100;
        printf("Modified element at index 5: %d\n", arr[5]);
    }

    int *p = arr + 3;
    if (p - arr >= 0 && p - arr < 10) {
        printf("Pointer points to index: %ld\n", p - arr);
    }

    for (int *p = arr; p < arr + 10; p++) {
        printf("%d ", *p);
    }
    printf("\n");

    return 0;
}