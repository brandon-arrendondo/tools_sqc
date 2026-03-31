/*
 * Rule: FIO08-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO08-C violation
 * Description: File closed before calling remove()
 */

#include <stdio.h>

void no_file_conflict(void) {
    FILE *fp = fopen("output.txt", "w");
    if (fp == NULL) return;
    fprintf(fp, "data\n");
    fclose(fp);

    /* Different file removed — never opened */
    remove("oldfile.txt");
}
