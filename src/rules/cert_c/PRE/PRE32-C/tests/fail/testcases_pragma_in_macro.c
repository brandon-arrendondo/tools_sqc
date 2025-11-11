/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #pragma directive used in macro argument
 */

#define COMPUTE(x) ((x) + 100)

void func(void) {
    int result = COMPUTE(  // Line 10 - VIOLATION
    #pragma message("Computing value")
        42
    );
}

int main(void) {
    func();
    return 0;
}
