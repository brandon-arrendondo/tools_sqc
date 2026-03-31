/*
 * Rule: FIO46-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO46-C violation
 *
 * File closed then reopened before next use
 */

#include <stdio.h>

void close_and_reopen(void) {
    FILE *fp = fopen("log.txt", "w");
    if (fp == NULL) return;

    fprintf(fp, "first write\n");
    fclose(fp);

    /* COMPLIANT: reopened before next use */
    fp = fopen("log.txt", "a");
    if (fp == NULL) return;
    fprintf(fp, "second write\n");
    fclose(fp);
}
