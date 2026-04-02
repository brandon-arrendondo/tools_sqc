/*
 * Rule: POS54-C
 * Source: testcases
 * Status: FAIL - Should trigger POS54-C violation
 *
 * POSIX function return value not checked
 */

#include <stdio.h>

void unchecked_fmemopen(void) {
    char buf[256];
    /* VIOLATION: return value of fmemopen not checked for NULL */
    FILE *fp = fmemopen(buf, sizeof(buf), "w");
    fprintf(fp, "data\n");
    fclose(fp);
}
