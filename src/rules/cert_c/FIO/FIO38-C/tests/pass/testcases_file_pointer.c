/*
 * Rule: FIO38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO38-C violation
 * Description: Using FILE pointers (not copies) is correct
 */

#include <stdio.h>

void use_file_pointer(void) {
    FILE *out = stdout;         /* Pointer assignment, not copy */
    fprintf(out, "hello\n");

    FILE *fp = fopen("test.txt", "w");
    if (fp != NULL) {
        FILE *alias = fp;       /* Pointer alias, not copy */
        fprintf(alias, "data\n");
        fclose(fp);
    }
}
