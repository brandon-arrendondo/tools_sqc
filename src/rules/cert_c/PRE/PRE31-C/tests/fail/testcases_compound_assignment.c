/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Compound assignment in unsafe macro
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

void func(int n, int delta) {
    // Compound assignment has side effect
    int result = ABS(n += delta);  // Line 11 - VIOLATION
}

int main(void) {
    func(10, 5);
    return 0;
}
