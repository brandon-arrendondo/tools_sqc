/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Nested function calls should not flag outer call
 * Regression: Round 8 fix — srand(time(NULL)) was incorrectly flagged
 */

#include <stdlib.h>
#include <time.h>

void seed_random(void) {
    srand(time(NULL));
}

void nested_calls(void) {
    srand((unsigned)time(NULL));
    abs(atoi("42"));
}
