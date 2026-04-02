/*
 * Rule: FIO46-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO46-C violation
 *
 * fprintf() on a stream after fclose()
 */

#include <stdio.h>

void write_after_close(void) {
    FILE *fp = fopen("log.txt", "w");
    if (fp == NULL) return;

    fprintf(fp, "first write\n");
    fclose(fp);

    /* VIOLATION: writing to closed file stream */
    fprintf(fp, "second write\n");
}
