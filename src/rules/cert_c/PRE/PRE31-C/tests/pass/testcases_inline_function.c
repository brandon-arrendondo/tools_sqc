/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: Using inline function instead of unsafe macro
 */

// Inline function - safe with side effects
static inline int iabs(int x) {
    return (((x) < 0) ? -(x) : (x));
}

void func(int n) {
    // Inline function evaluates argument once - COMPLIANT
    int m = iabs(++n);
}

int main(void) {
    func(5);
    return 0;
}
