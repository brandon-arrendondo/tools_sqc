/*
 * Rule: FIO13-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO13-C violation
 *
 * Single ungetc() call is always compliant
 */

#include <stdio.h>

void single_ungetc(FILE *fp) {
    int ch = fgetc(fp);
    /* COMPLIANT: only one ungetc, no successive pushback */
    if (ch != '\n') {
        ungetc(ch, fp);
    }
}
