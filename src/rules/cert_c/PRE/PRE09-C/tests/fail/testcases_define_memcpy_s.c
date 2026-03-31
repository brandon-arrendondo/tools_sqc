/*
 * Rule: PRE09-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE09-C violation
 *
 * Macro replaces memcpy_s with less secure memcpy
 */

#include <string.h>

/* VIOLATION: replacing bounds-checked memcpy_s with unchecked memcpy */
#define memcpy_s(dest, destsz, src, count) memcpy(dest, src, count)

void copy_data(char *dst, size_t dst_size, const char *src, size_t len) {
    memcpy_s(dst, dst_size, src, len);
}
