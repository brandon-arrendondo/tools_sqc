/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #error directive used in macro argument
 */

#define PROCESS(x) ((x) * 2)

void func(void) {
    int result = PROCESS(  // Line 10 - VIOLATION
    #ifndef REQUIRED_MACRO
    #error "REQUIRED_MACRO must be defined"
    #endif
        10
    );
}

int main(void) {
    return 0;
}
