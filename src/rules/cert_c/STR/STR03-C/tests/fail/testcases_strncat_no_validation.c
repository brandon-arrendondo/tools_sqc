/*
 * Rule: STR03-C
 * Source: testcases
 * Status: FAIL - strncat/snprintf without length validation
 */

#include <string.h>
#include <stdio.h>

/* strncat without checking remaining space */
void unsafe_concat(char *dest, const char *src) {
    strncat(dest, src, 100);
}

/* snprintf without checking return value */
void unsafe_format(char *buf, int value) {
    snprintf(buf, 256, "value=%d", value);
}
