/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #undef directive used in macro argument
 */

#define TEMP 100
#define PROCESS(x) ((x) * 2)

void func(void) {
    int result = PROCESS(  // Line 11 - VIOLATION
    #undef TEMP
        50
    );
}

int main(void) {
    func();
    return 0;
}
