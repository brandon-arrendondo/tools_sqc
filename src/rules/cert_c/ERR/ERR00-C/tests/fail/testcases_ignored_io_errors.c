/*
 * Rule: ERR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR00-C violation
 * Description: I/O function return values ignored
 */

#include <stdio.h>

void write_data(const char *msg) {
    FILE *fp = fopen("out.txt", "w");  /* No NULL check */
    fprintf(fp, "%s\n", msg);
    fclose(fp);  /* Return value ignored */
}
