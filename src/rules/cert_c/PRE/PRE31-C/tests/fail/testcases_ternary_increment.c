/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Ternary expression with increment in unsafe macro
 */

#define DOUBLE(x) ((x) * 2)  /* UNSAFE if used with x*2 instead of (x)*2 */
#define SQUARE(x) ((x) * (x))  /* UNSAFE */

void ternary_test(int a, int b) {
    // Ternary with increment evaluated multiple times
    int result = SQUARE(a > b ? ++a : ++b);  // Line 12 - VIOLATION
}

int main(void) {
    ternary_test(5, 10);
    return 0;
}
