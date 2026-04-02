/*
 * Rule: ERR33-C
 * Status: FAIL - fopen return value not checked
 */

#include <stdio.h>

void f(const char *filename) {
    FILE *fp = fopen(filename, "r");
    /* Missing NULL check on fp */
    fprintf(fp, "test\n");
    fclose(fp);
}
