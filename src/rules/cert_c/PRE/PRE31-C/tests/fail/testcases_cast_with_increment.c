/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Cast with increment in unsafe macro
 */

#define TO_UNSIGNED(x) ((unsigned int)(x))  /* UNSAFE if x evaluated twice */
#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

void cast_test(int val) {
    // Cast with increment has side effect
    unsigned int result = TO_UNSIGNED(ABS(++val));  // Line 12 - VIOLATION
}

int main(void) {
    cast_test(-5);
    return 0;
}
