/*
 * Rule: FIO11-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO11-C violation
 *
 * C11 exclusive create modes are valid standard mode strings
 */

#include <stdio.h>

void compliant_c11_exclusive_modes(void) {
    FILE *fp;

    /* COMPLIANT: C11 exclusive create modes */
    fp = fopen("file.txt", "wx");
    if (fp) fclose(fp);

    fp = fopen("file.bin", "wbx");
    if (fp) fclose(fp);

    fp = fopen("file.txt", "w+x");
    if (fp) fclose(fp);

    fp = fopen("file.bin", "w+bx");
    if (fp) fclose(fp);

    fp = fopen("file.bin", "wb+x");
    if (fp) fclose(fp);
}
