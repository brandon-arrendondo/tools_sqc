/*
 * Rule: PRE09-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE09-C violation
 *
 * Wrapper function instead of macro replacement
 */

#include <stdio.h>
#include <string.h>

/* COMPLIANT: wrapper function, not a macro replacing a secure function */
int safe_snprintf(char *buf, size_t size, const char *fmt, ...) {
    /* ... custom implementation ... */
    return 0;
}

void format_message(char *buf, size_t len) {
    safe_snprintf(buf, len, "value: %d", 42);
}
