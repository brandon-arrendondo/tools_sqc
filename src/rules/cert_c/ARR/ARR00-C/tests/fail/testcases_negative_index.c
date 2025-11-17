/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int arr[10] = {0};

    arr[-1] = 42;

    int value = arr[-5];
    printf("Value at negative index: %d\n", value);

    return 0;
}