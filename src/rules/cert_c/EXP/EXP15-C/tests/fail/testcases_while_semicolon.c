/*
 * Rule: EXP15-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP15-C violation
 *
 * while statement with empty body (bare semicolon)
 */

#include <stdio.h>

void while_empty_body(int *data, int n) {
    int i = 0;
    /* VIOLATION: semicolon makes while body empty */
    while (i < n);
    {
        data[i] = 0;
        i++;
    }
}
