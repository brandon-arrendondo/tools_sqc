/*
 * Rule: FIO14-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO14-C violation
 *
 * fseek() with ftell() return value is compliant
 */

#include <stdio.h>

void seek_with_ftell(FILE *fp) {
    /* COMPLIANT: offset from ftell() is always valid for SEEK_SET */
    long pos = ftell(fp);
    /* ... process some data ... */
    fseek(fp, pos, SEEK_SET);
}
