/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Comma expression with increment in unsafe macro
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

void comma_test(int a, int b) {
    // Comma expression with side effect
    int result = ABS((++a, b));  // Line 11 - VIOLATION
}

int main(void) {
    comma_test(5, 10);
    return 0;
}
