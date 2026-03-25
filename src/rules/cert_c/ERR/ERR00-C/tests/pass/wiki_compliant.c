/*
 * Rule: ERR00-C
 * Source: wiki
 * Status: PASS - Return values checked for errors
 */

#include <stdio.h>

int read_data(void) {
    int value;
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) {
        return -1;
    }
    int rc = fscanf(fp, "%d", &value);
    if (rc != 1) {
        (void)fclose(fp);
        return -1;
    }
    if (fclose(fp) != 0) {
        return -1;
    }
    return 0;
}
