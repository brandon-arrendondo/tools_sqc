/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #if directive used in strcmp() function argument
 */

#include <string.h>

void compare_strings(const char *s1) {
    int result = strcmp(s1,  // Line 10 - VIOLATION
    #if defined(PLATFORM_A)
        "platform_a"
    #elif defined(PLATFORM_B)
        "platform_b"
    #else
        "default"
    #endif
    );
}

int main(void) {
    compare_strings("test");
    return 0;
}
