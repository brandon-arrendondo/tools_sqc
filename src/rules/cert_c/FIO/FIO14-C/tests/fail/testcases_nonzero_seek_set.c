/*
 * Rule: FIO14-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO14-C violation
 *
 * fseek() with non-zero literal offset and SEEK_SET on text stream
 */

#include <stdio.h>

void nonzero_seek_set(FILE *fp) {
    /* VIOLATION: non-zero literal offset with SEEK_SET is not portable */
    fseek(fp, 100, SEEK_SET);
}
