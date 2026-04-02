/*
 * Rule: WIN04-C
 * Source: testcases
 * Status: FAIL - Should trigger WIN04-C violation
 *
 * Function pointer stored without encryption
 */

#include <stdio.h>

void testcase_unencrypted_fnptr(void) {
    int (*log_fn)(const char *, ...) = printf;
    /* VIOLATION: function pointer not encrypted */
    log_fn("hello");
}
