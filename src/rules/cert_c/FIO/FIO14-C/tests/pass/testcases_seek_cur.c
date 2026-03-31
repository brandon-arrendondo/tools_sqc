/*
 * Rule: FIO14-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO14-C violation
 *
 * fseek() with SEEK_CUR is compliant
 */

#include <stdio.h>

void seek_cur_relative(FILE *fp) {
    /* COMPLIANT: SEEK_CUR with any offset is fine */
    fseek(fp, 10, SEEK_CUR);
    fseek(fp, -5, SEEK_CUR);
}
