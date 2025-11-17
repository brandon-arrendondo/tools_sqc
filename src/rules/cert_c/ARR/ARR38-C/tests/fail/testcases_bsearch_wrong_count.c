/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: bsearch with count exceeding array size
 */

#include <stdlib.h>

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void bsearch_exceed(void) {
    int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int key = 5;

    // Claims array has 50 elements but it only has 10
    bsearch(&key, arr, 50, sizeof(int), compare);  // Line 17 - VIOLATION
}

int main(void) {
    bsearch_exceed();
    return 0;
}
