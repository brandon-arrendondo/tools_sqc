/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in assert() macro argument
 */

#include <assert.h>

void check_value(int x) {
    assert(x >  // Line 10 - VIOLATION
    #ifdef STRICT
        100
    #else
        50
    #endif
    );
}

int main(void) {
    check_value(75);
    return 0;
}
