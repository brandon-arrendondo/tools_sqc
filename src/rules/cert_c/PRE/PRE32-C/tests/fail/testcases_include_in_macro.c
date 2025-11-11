/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #include directive used in macro argument
 */

#define PROCESS(x) ((x) * 2)

void func(void) {
    int result = PROCESS(  // Line 10 - VIOLATION
    #include "config.h"
        CONFIG_VALUE
    );
}

int main(void) {
    return 0;
}
