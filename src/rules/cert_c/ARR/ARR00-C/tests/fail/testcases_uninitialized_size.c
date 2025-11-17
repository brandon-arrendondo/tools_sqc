/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int size;
    int arr[10];

    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    return 0;
}