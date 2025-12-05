/*
 * Rule: FIO11-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO11-C violation
 *
 * Using valid C standard fopen() mode strings
 */

#include <stdio.h>

void compliant_fopen_modes(void) {
    FILE *fp;

    /* COMPLIANT: Standard mode strings */
    fp = fopen("file.txt", "r");
    if (fp) fclose(fp);

    fp = fopen("file.txt", "w");
    if (fp) fclose(fp);

    fp = fopen("file.txt", "a");
    if (fp) fclose(fp);

    fp = fopen("file.bin", "rb");
    if (fp) fclose(fp);

    fp = fopen("file.bin", "wb");
    if (fp) fclose(fp);

    fp = fopen("file.txt", "r+");
    if (fp) fclose(fp);
}
