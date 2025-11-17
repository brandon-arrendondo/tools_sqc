/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in memcpy() function argument
 */

#include <string.h>

void func(const char *src) {
    char *dest;
    memcpy(dest, src,  // Line 11 - VIOLATION
    #ifdef PLATFORM1
        12
    #else
        24
    #endif
    );
}

int main(void) {
    return 0;
}
