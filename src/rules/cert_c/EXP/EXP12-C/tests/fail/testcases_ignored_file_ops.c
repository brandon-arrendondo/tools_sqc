/*
 * Rule: EXP12-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP12-C violation
 * Description: Return values from file operations ignored
 */

#include <stdio.h>

void ignore_file_returns(void) {
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) return;

    fclose(fp);      /* Violation: return value ignored */
    remove("old.txt"); /* Violation: return value ignored */
    rename("a.txt", "b.txt"); /* Violation: return value ignored */
}
