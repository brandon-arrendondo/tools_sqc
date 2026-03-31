/*
 * Rule: EXP12-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP12-C violation
 * Description: Return values properly checked or assigned
 */

#include <stdio.h>
#include <stdlib.h>

int properly_checked(void) {
    char *buf = malloc(256);
    if (buf == NULL) return -1;

    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) {
        free(buf);
        buf = NULL;
        return -1;
    }

    int rc = fclose(fp);
    if (rc != 0) return -1;

    if (remove("old.txt") != 0) {
        /* handle error */
    }

    free(buf);
    buf = NULL;
    return 0;
}
