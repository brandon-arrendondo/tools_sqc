/*
 * Rule: FIO46-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO46-C violation
 *
 * All file operations occur before fclose()
 */

#include <stdio.h>

void proper_file_usage(void) {
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) return;

    char buf[256];
    /* COMPLIANT: all reads before close */
    fread(buf, 1, sizeof(buf), fp);
    fclose(fp);
}
