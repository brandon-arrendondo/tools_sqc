/*
 * Rule: EXP09-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP09-C violation
 * Description: Hardcoded numeric sizes in allocation calls
 */

#include <stdlib.h>

void allocate_with_magic_numbers(void) {
    int *arr = (int *)malloc(40);       /* Violation: 40 instead of sizeof */
    double *d = (double *)malloc(8);    /* Violation: 8 instead of sizeof */
    long *p = (long *)calloc(10, 8);    /* Violation: 8 instead of sizeof */

    free(p);
    free(d);
    free(arr);
}
