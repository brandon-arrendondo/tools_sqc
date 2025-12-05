/*
 * Rule: FIO11-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO11-C violation
 *
 * Using non-standard fopen() mode string
 */

#include <stdio.h>

void noncompliant_fopen_mode(void) {
    /* VIOLATION: "rw" is not a valid C standard mode string */
    FILE *fp = fopen("file.txt", "rw");
    if (fp != NULL) {
        fclose(fp);
    }
}
