/*
 * Rule: EXP12-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP12-C violation
 * Description: Explicit (void) cast signals intentional discard
 */

#include <stdio.h>
#include <stdlib.h>

void intentional_discard(void) {
    FILE *fp = fopen("data.txt", "w");
    if (fp == NULL) return;

    (void)fprintf(fp, "hello\n");
    (void)fclose(fp);
    (void)remove("tmp.txt");
}
