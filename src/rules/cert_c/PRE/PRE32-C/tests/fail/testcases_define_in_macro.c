/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #define directive used in macro argument
 */

#define ABS(x) ((x) < 0 ? -(x) : (x))

void func(void) {
    int result = ABS(  // Line 10 - VIOLATION
    #define TEMP_VAL 10
        TEMP_VAL
    );
}

int main(void) {
    func();
    return 0;
}
