/*
 * Rule: FIO11-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO11-C violation
 *
 * fopen() with mixed invalid mode string "rt" (text mode is not standard C)
 */

#include <stdio.h>

void noncompliant_text_mode(void) {
    /* VIOLATION: "rt" is a Microsoft extension, not standard C */
    FILE *fp = fopen("file.txt", "rt");
    if (fp != NULL) {
        fclose(fp);
    }
}
