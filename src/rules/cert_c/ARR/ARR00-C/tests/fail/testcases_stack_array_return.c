/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int* get_array() {
    int local_array[10];

    for (int i = 0; i < 10; i++) {
        local_array[i] = i;
    }

    return local_array;
}

int main() {
    int *arr = get_array();

    printf("%d\n", arr[0]);

    return 0;
}