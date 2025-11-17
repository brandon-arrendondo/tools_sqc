/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int arr[5] = {1, 2, 3, 4, 5};

    int value = arr[5];
    printf("Value: %d\n", value);

    return 0;
}