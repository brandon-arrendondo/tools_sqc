/*
 * Rule: ERR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR00-C violation
 * Description: I/O operations with proper error checking
 */

#include <stdio.h>

int safe_write(const char *msg) {
    FILE *fp = fopen("out.txt", "w");
    if (fp == NULL) return -1;

    int rc = fprintf(fp, "%s\n", msg);
    if (rc < 0) {
        if (fclose(fp) != 0) { /* handle */ }
        return -1;
    }

    if (fclose(fp) != 0) return -1;

    return 0;
}
