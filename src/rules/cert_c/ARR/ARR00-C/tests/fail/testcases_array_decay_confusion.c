/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void modify_array(int arr[100]) {
    size_t supposed_size = sizeof(arr);
    printf("Size in function: %zu\n", supposed_size);

    for (int i = 0; i < 100; i++) {
        arr[i] = i;
    }
}

int main() {
    int *dynamic = malloc(10 * sizeof(int));

    modify_array(dynamic);

    free(dynamic);
    return 0;
}