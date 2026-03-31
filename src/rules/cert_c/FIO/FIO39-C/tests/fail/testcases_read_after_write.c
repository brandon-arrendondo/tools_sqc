/*
 * Rule: FIO39-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO39-C violation
 * Description: Input after output without intervening positioning call
 */

#include <stdio.h>

void read_write_no_flush(FILE *fp) {
    char buf[64];

    fputs("data\n", fp);
    fgets(buf, sizeof(buf), fp);  /* Violation: no fseek/fflush between */
}
