/*
 * Rule: MEM07-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM07-C violation
 *
 * calloc() without overflow check for size arguments
 */

#include <stdlib.h>

void allocate_array(size_t num_elements) {
    /* VIOLATION: no overflow check before calloc */
    long *arr = (long *)calloc(num_elements, sizeof(long));
    if (arr == NULL) {
        return;
    }
    arr[0] = 1;
    free(arr);
}
