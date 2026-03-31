/*
 * Rule: EXP09-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP09-C violation
 * Description: Proper sizeof usage in allocation
 */

#include <stdlib.h>

void allocate_with_sizeof(int count) {
    int *arr = malloc(count * sizeof(int));
    double *d = malloc(sizeof(double));
    long *p = calloc(count, sizeof(long));
    char *buf = malloc(sizeof(*buf) * 256);

    free(buf);
    free(p);
    free(d);
    free(arr);
}
