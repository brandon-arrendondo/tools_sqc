/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #line directive used in macro argument
 */

#define PROCESS(x) ((x) * 3)

void func(void) {
    int result = PROCESS(  // Line 10 - VIOLATION
    #line 100 "fake.c"
        42
    );
}

int main(void) {
    func();
    return 0;
}
