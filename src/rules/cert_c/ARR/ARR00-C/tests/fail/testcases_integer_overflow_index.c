/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <limits.h>

int main() {
    int arr[100];
    unsigned int index = UINT_MAX;

    index++;
    arr[index] = 42;

    return 0;
}