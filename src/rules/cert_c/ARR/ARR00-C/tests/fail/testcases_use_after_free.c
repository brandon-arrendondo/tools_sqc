/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *arr = malloc(10 * sizeof(int));

    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }

    free(arr);

    arr[5] = 100;

    return 0;
}