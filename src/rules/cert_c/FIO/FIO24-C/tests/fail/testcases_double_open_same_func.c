/*
 * Rule: FIO24-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO24-C violation
 * Description: Same file opened twice without closing first
 */

#include <stdio.h>

void double_open(void) {
    FILE *fp1 = fopen("config.txt", "r");
    if (fp1 == NULL) return;

    FILE *fp2 = fopen("config.txt", "w");  /* Violation: already open */
    if (fp2 == NULL) {
        fclose(fp1);
        return;
    }

    fclose(fp2);
    fclose(fp1);
}
