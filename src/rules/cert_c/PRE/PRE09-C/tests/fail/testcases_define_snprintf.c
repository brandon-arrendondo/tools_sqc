/*
 * Rule: PRE09-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE09-C violation
 *
 * Macro replaces snprintf with less secure sprintf
 */

#include <stdio.h>

/* VIOLATION: replacing secure snprintf with insecure sprintf */
#define snprintf(buf, size, fmt, ...) sprintf(buf, fmt, __VA_ARGS__)

void format_message(char *buf, size_t len) {
    snprintf(buf, len, "value: %d", 42);
}
