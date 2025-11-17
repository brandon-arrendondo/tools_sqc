/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: qsort with correct element size
 */

#include <stdlib.h>
#include <stdio.h>

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void qsort_proper(void) {
    int arr[10] = {5, 2, 9, 1, 7, 6, 3, 8, 4, 0};

    // Use sizeof(*arr) or sizeof(int) for int array - COMPLIANT
    qsort(arr, 10, sizeof(*arr), compare);

    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    qsort_proper();
    return 0;
}
