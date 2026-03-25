/*
 * Rule: DCL09-C
 * Source: testcases
 * Status: PASS - Correct errno_t usage and non-errno returns
 */

#include <errno.h>

typedef int errno_t;

/* Correct: errno_t return type */
errno_t safe_divide(int a, int b) {
    if (b == 0) return EINVAL;
    return 0;
}

/* Not an errno function — returns regular int */
int compute(int x) {
    return x * 2;
}

/* Returns 0 and 1 — not errno constants */
int is_valid(int x) {
    return x > 0 ? 1 : 0;
}
