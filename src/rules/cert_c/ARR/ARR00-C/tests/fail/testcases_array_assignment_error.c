/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int arr1[10];
    int arr2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    arr1 = arr2;

    return 0;
}