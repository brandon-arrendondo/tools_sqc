/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in nested macro argument
 */

#define OUTER(x) ((x) + 10)
#define INNER(y) ((y) * 2)

void compute(int n) {
    int result = OUTER(INNER(  // Line 11 - VIOLATION
    #ifdef BOOST
        n * 3
    #else
        n
    #endif
    ));
}

int main(void) {
    compute(5);
    return 0;
}
