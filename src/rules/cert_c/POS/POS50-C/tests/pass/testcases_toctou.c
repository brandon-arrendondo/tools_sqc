/*
 * Rule: POS50-C
 * Source: testcases
 * Status: PASS - Known limitation: stat()+fopen() TOCTOU not detected
 * TODO: Move to fail/ when TOCTOU pattern detection is implemented (see PLAN.md)
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
