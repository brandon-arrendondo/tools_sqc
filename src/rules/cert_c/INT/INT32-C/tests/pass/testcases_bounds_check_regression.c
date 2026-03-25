/*
 * Rule: INT32-C
 * Source: testcases
 * Status: PASS - Arithmetic inside bounds-checked blocks should not flag
 * Regression: Round 11 fix — guarded arithmetic was incorrectly flagged
 */

#include <limits.h>

int safe_add(int a, int b) {
    if (a > 0 && b > 0 && a > INT_MAX - b) {
        return -1;
    }
    return a + b;
}

int safe_multiply(int a, int b) {
    if (b != 0 && a > INT_MAX / b) {
        return -1;
    }
    return a * b;
}

short safe_add_short(short a, short b) {
    if (a > SHRT_MAX - b) {
        return -1;
    }
    return a + b;
}
