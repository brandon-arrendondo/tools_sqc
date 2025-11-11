/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #elif directive used in macro argument
 */

#define ADD(x, y) ((x) + (y))

void compute(int n) {
    int result = ADD(n,  // Line 10 - VIOLATION
    #if defined(MODE_A)
        10
    #elif defined(MODE_B)
        20
    #else
        30
    #endif
    );
}

int main(void) {
    compute(5);
    return 0;
}
