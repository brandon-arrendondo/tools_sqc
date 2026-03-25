/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Pointer arithmetic is not unsigned integer overflow
 * Regression: Round 9 fix — ptr + n was incorrectly flagged
 */

#include <stddef.h>

void pointer_ops(char *buf, size_t len) {
    char *end = buf + len;
    char *mid = buf + len / 2;
    ptrdiff_t diff = end - buf;
    buf += 10;
    buf -= 5;
    (void)end;
    (void)mid;
    (void)diff;
}
