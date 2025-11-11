/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Nested macro calls with increment
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */
#define DOUBLE(x) ((x) + (x))  /* UNSAFE */

void nested_test(int val) {
    // Nested macros with increment - multiple evaluations
    int result = DOUBLE(ABS(++val));  // Line 12 - VIOLATION
}

int main(void) {
    nested_test(5);
    return 0;
}
