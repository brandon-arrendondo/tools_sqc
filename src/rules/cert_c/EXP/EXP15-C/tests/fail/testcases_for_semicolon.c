/*
 * Rule: EXP15-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP15-C violation
 *
 * for statement with empty body (bare semicolon)
 */

#include <stdio.h>

void for_empty_body(int *arr, int n) {
    /* VIOLATION: semicolon makes for-loop body empty */
    for (int i = 0; i < n; i++);
    {
        arr[0] = 42;
    }
}
