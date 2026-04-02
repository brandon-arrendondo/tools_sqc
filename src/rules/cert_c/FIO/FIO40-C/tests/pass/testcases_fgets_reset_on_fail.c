/*
 * Rule: FIO40-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO40-C violation
 *
 * fgets() failure with proper buffer reset
 */

#include <stdio.h>

void fgets_reset_on_failure(FILE *fp) {
    char buf[256];
    /* COMPLIANT: buffer reset in failure branch */
    if (fgets(buf, sizeof(buf), fp) == NULL) {
        buf[0] = '\0';
    }
    printf("%s", buf);
}
