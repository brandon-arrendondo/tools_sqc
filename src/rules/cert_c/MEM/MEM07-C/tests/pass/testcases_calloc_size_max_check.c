/*
 * Rule: MEM07-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM07-C violation
 *
 * calloc() with SIZE_MAX overflow check
 */

#include <stdlib.h>
#include <stdint.h>

void allocate_array_safe(size_t num_elements) {
    /* COMPLIANT: check for overflow before allocation */
    if (num_elements > SIZE_MAX / sizeof(long)) {
        return;
    }
    long *arr = (long *)calloc(num_elements, sizeof(long));
    if (arr == NULL) {
        return;
    }
    arr[0] = 1;
    free(arr);
}
