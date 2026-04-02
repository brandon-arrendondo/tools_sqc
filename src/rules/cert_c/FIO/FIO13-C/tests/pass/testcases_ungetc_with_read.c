/*
 * Rule: FIO13-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO13-C violation
 *
 * ungetc() calls separated by read operations
 */

#include <stdio.h>

void ungetc_with_intervening_read(FILE *fp) {
    int ch;

    /* COMPLIANT: read between each ungetc */
    ungetc('a', fp);
    ch = fgetc(fp);

    ungetc('b', fp);
    ch = fgetc(fp);
}
