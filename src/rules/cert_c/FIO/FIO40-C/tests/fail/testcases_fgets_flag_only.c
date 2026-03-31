/*
 * Rule: FIO40-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO40-C violation
 *
 * fgets() failure sets error flag but does not reset buffer
 */

#include <stdio.h>

void fgets_flag_no_reset(FILE *fp) {
    char buf[256];
    int error_flag = 0;
    /* VIOLATION: buffer not reset on failure, only flag set */
    if (fgets(buf, sizeof(buf), fp) == NULL) {
        error_flag = 1;
    }
    if (!error_flag) {
        printf("%s", buf);
    }
}
