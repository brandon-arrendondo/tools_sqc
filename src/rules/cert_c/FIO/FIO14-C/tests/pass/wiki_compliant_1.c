/*
 * Rule: FIO14-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO14-C violation
 *
 * Using fseek() with SEEK_SET and offset 0 is safe
 */

#include <stdio.h>

void compliant_seek_start(void) {
    FILE *fp = fopen("data.bin", "rb");
    if (fp != NULL) {
        /* COMPLIANT: fseek() with SEEK_SET and offset 0 */
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
    }
}
