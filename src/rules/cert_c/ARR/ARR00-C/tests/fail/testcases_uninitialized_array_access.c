/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int arr[10];

    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }

    return 0;
}