/*
 * Rule: FIO39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO39-C violation
 * Description: Proper positioning call between I/O direction changes
 */

#include <stdio.h>

void write_then_read(FILE *fp) {
    char buf[64];

    fputs("data\n", fp);
    fflush(fp);
    fgets(buf, sizeof(buf), fp);  /* Safe: fflush between */
}

void write_seek_read(FILE *fp) {
    char buf[64];

    fwrite("test", 1, 4, fp);
    fseek(fp, 0L, 0);
    fread(buf, 1, sizeof(buf), fp);  /* Safe: fseek between */
}
