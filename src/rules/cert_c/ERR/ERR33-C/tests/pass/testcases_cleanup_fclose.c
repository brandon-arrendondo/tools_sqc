/*
 * Rule: ERR33-C
 * Status: PASS - fclose in cleanup/error-handling context
 */

#include <stdio.h>

int f(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    char buf[256];
    if (fgets(buf, sizeof(buf), fp) == NULL) {
        /* Error path: fclose in cleanup context */
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}
