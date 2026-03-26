/*
 * Rule: POS50-C
 * Source: testcases
 * Status: FAIL - stat()+fopen() TOCTOU race condition
 */

#include <stdio.h>
#include <sys/stat.h>

/* access() then open() — classic TOCTOU */
void toctou_access_open(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        FILE *f = fopen(path, "r");
        if (f) fclose(f);
    }
}
