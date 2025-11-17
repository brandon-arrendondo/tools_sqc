/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Conditional value computed before macro call
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void compute(int x, int y) {
    // Compliant: conditional resolved to variable first
#ifdef USE_DOUBLE
    int y_val = y * 2;
#else
    int y_val = y;
#endif
    int result = MAX(x, y_val);
}

int main(void) {
    compute(10, 20);
    return 0;
}
