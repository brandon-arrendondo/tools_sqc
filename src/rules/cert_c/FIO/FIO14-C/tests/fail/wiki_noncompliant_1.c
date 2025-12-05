/*
 * Rule: FIO14-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO14-C violation
 *
 * Using fseek() with SEEK_END on binary stream is undefined behavior
 */

#include <stdio.h>

void noncompliant_binary_seek_end(void) {
    FILE *fp = fopen("data.bin", "rb");
    if (fp != NULL) {
        /* VIOLATION: fseek() with SEEK_END on binary stream */
        fseek(fp, 0, SEEK_END);
        fclose(fp);
    }
}
