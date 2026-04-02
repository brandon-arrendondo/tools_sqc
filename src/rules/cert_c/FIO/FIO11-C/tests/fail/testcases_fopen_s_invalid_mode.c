/*
 * Rule: FIO11-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO11-C violation
 *
 * fopen_s() with non-standard mode string
 */

#include <stdio.h>

void noncompliant_fopen_s_mode(void) {
    FILE *fp;
    /* VIOLATION: "wr" is not a valid C standard mode string */
    errno_t err = fopen_s(&fp, "data.bin", "wr");
    if (err == 0 && fp != NULL) {
        fclose(fp);
    }
}
