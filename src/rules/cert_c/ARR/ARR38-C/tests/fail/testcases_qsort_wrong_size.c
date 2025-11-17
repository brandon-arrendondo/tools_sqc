/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: qsort with wrong element size
 */

#include <stdlib.h>

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void qsort_incorrect(void) {
    int arr[10] = {5, 2, 9, 1, 7, 6, 3, 8, 4, 0};

    // Wrong: using sizeof(long) for int array
    qsort(arr, 10, sizeof(long), compare);  // Line 17 - VIOLATION
}

int main(void) {
    qsort_incorrect();
    return 0;
}
