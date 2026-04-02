/*
 * Rule: ERR33-C
 * Status: PASS - fprintf to stderr in error handling context (no other unchecked calls)
 */

#include <stdio.h>
#include <stdlib.h>

void f(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open %s\n", filename);
        return;
    }
    if (fclose(fp) != 0) {
        fprintf(stderr, "Error closing file\n");
    }
}
