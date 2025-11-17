/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Accesses array elements after the array has been freed
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *arr = malloc(5 * sizeof(int));
    if (arr == NULL) {
        return -1;
    }

    for (int i = 0; i < 5; i++) {
        arr[i] = i + 1;
    }

    free(arr);

    // BUG: Access freed array
    for (int i = 0; i < 5; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    return 0;
}