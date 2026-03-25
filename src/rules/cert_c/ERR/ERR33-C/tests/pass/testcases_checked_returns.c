/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Return values properly checked
 */

#include <stdio.h>
#include <stdlib.h>

/* Checked fopen and fclose */
void checked_fopen(const char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return;
    }
    if (fclose(f) != 0) {
        return;
    }
}

/* Checked malloc */
void checked_malloc(int n) {
    int *p = (int *)malloc(n * sizeof(int));
    if (p == NULL) {
        return;
    }
    p[0] = 42;
    free(p);
}

/* No critical function calls */
int simple_add(int a, int b) {
    return a + b;
}
