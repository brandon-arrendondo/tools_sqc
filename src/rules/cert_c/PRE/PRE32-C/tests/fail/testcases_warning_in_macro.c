/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #warning directive used in macro argument
 */

#define CALC(x) ((x) + 50)

void func(void) {
    int result = CALC(  // Line 10 - VIOLATION
    #warning "Deprecated configuration"
        25
    );
}

int main(void) {
    func();
    return 0;
}
