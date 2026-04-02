/*
 * Rule: FIO50-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO50-C violation
 *
 * fflush() between output and input operations
 */

#include <stdio.h>

void fflush_between_io(FILE *fp) {
    int value;
    /* COMPLIANT: fflush separates output from input */
    fprintf(fp, "data: %d\n", 42);
    fflush(fp);
    fscanf(fp, "%d", &value);
}
