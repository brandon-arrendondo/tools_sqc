/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Unchecked return values from critical functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Unchecked fopen */
void unchecked_fopen(const char *path) {
    FILE *f = fopen(path, "r");
    fprintf(f, "data\n");
    fclose(f);
}

/* Unchecked malloc */
void unchecked_malloc(int n) {
    int *p = (int *)malloc(n * sizeof(int));
    p[0] = 42;
    free(p);
}

/* Unchecked fclose */
void unchecked_fclose(FILE *f) {
    fclose(f);
}

/* Unchecked fread */
void unchecked_fread(FILE *f) {
    char buf[100];
    fread(buf, 1, 100, f);
    (void)buf;
}
