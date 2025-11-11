/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in custom macro argument
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void compute(int x, int y) {
    int result = MAX(x,  // Line 10 - VIOLATION
    #ifdef USE_DOUBLE
        y * 2
    #else
        y
    #endif
    );
}

int main(void) {
    compute(10, 20);
    return 0;
}
