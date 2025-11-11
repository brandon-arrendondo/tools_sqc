/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Multiplying pointer difference by sizeof
 */

#include <stddef.h>

void ptr_diff_scale(void) {
    int array[50];
    int *start = &array[10];
    int *end = &array[30];

    ptrdiff_t diff = end - start;
    // Manually scaling difference by sizeof
    int *ptr = start + (diff * sizeof(int));  // Line 15 - VIOLATION

    *ptr = 100;
}

int main(void) {
    ptr_diff_scale();
    return 0;
}
