/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifndef directive used in macro argument
 */

#define SQUARE(x) ((x) * (x))

void calculate(int n) {
    int result = SQUARE(  // Line 10 - VIOLATION
    #ifndef MINIMAL
        n + 5
    #else
        n
    #endif
    );
}

int main(void) {
    calculate(10);
    return 0;
}
