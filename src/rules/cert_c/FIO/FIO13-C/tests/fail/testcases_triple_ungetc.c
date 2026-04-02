/*
 * Rule: FIO13-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO13-C violation
 *
 * Three successive ungetc() calls without intervening read
 */

#include <stdio.h>

void triple_ungetc(FILE *fp) {
    /* VIOLATION: successive ungetc without read between */
    ungetc('a', fp);
    ungetc('b', fp);
    ungetc('c', fp);
}
