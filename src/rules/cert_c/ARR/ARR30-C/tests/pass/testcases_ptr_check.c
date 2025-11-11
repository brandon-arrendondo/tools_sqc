/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Pointer arithmetic is validated against array bounds
 */

#include <stdio.h>

int main(void) {
    int data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int *ptr = data;
    int *end = data + 8;

    // Safe pointer traversal with bounds checking
    while (ptr < end) {
        printf("%d ", *ptr);
        ptr++;
    }
    printf("\n");

    return 0;
}