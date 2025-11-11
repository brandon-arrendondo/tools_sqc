/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in strncmp() function argument
 */

#include <string.h>

void compare_prefix(const char *s1, const char *s2) {
    int result = strncmp(s1, s2,  // Line 10 - VIOLATION
    #ifdef LONG_COMPARE
        64
    #else
        16
    #endif
    );
}

int main(void) {
    compare_prefix("test1", "test2");
    return 0;
}
