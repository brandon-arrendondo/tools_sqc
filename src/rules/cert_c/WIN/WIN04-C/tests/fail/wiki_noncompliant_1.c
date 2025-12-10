/*
 * Rule: WIN04-C
 * Source: wiki
 * Status: FAIL - Should trigger WIN04-C violation
 * Description: Function pointer stored unencrypted
 */

#include <stdio.h>

void testcase_noncompliant_unencrypted_fnptr(void) {
    int (*log_fn)(const char *, ...) = printf;  /* Violation: unencrypted function pointer */
    /* ... */
    log_fn("foo");
}
