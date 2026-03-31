/*
 * Rule: FIO24-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO24-C violation
 * Description: File properly closed before reopening
 */

#include <stdio.h>

void close_then_reopen(void) {
    FILE *fp = fopen("log.txt", "r");
    if (fp == NULL) return;

    /* Read phase */
    fclose(fp);

    /* Write phase - safe because file was closed */
    fp = fopen("log.txt", "w");
    if (fp == NULL) return;

    fprintf(fp, "new data\n");
    fclose(fp);
}
