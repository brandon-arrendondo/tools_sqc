/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Macro constant defined outside function-like macro
 */

#define SQUARE(x) ((x) * (x))

#ifndef MINIMAL
#define OFFSET 5
#else
#define OFFSET 0
#endif

void calculate(int n) {
    // Compliant: OFFSET resolved before SQUARE invocation
    int result = SQUARE(n + OFFSET);
}

int main(void) {
    calculate(10);
    return 0;
}
