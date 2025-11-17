/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Using inline function instead of macro allows conditionals
 */

#ifdef STRICT
#define THRESHOLD 100
#else
#define THRESHOLD 50
#endif

static inline int abs_value(int x) {
    return x < 0 ? -x : x;
}

void func(int n) {
    // Compliant: inline function, not a macro
    int m = abs_value(n - THRESHOLD);
}

int main(void) {
    func(75);
    return 0;
}
