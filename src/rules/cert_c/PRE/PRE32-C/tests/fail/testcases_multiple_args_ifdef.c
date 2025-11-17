/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in macro with multiple arguments
 */

#define MULTIPLY(a, b) ((a) * (b))

void calculate(int x) {
    int result = MULTIPLY(x,  // Line 10 - VIOLATION
    #ifdef TRIPLE
        3
    #else
        2
    #endif
    );
}

int main(void) {
    calculate(10);
    return 0;
}
