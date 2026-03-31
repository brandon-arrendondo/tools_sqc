/*
 * Rule: FIO08-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO08-C violation
 * Description: remove() called on file that is still open
 */

#include <stdio.h>

void update_and_remove(const char *path) {
    FILE *fp = fopen(path, "w");
    if (fp == NULL) return;

    fprintf(fp, "temporary data\n");
    remove(path);  /* Violation: file still open */
    fclose(fp);
}
