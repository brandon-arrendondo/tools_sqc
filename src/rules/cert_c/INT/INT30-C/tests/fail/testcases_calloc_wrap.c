/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Passing wrapped multiplication to calloc
 */

#include <stdlib.h>
#include <stddef.h>

void allocate_2d_array(size_t rows, size_t cols) {
    // rows * cols may wrap before calloc
    size_t total = rows * cols;  // Line 11 - VIOLATION

    int *array = (int *)calloc(total, sizeof(int));
    if (array) {
        free(array);
    }
}

int main(void) {
    allocate_2d_array(SIZE_MAX / 2, 10);  // Will wrap
    return 0;
}
