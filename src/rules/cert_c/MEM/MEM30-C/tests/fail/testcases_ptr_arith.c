/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Pointer arithmetic on freed memory
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *arr = malloc(5 * sizeof(int));
    if (arr == NULL) {
        return -1;
    }

    for (int i = 0; i < 5; i++) {
        arr[i] = i * 2;
    }

    free(arr);

    // BUG: Pointer arithmetic on freed memory
    int *ptr = arr + 2;
    printf("Value at offset 2: %d\n", *ptr);

    return 0;
}