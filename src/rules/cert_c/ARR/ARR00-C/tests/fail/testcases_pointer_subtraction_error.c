/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int arr1[10];
    int arr2[10];

    int *p1 = &arr1[5];
    int *p2 = &arr2[3];

    ptrdiff_t diff = p1 - p2;
    printf("Difference: %ld\n", diff);

    return 0;
}