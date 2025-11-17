/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in strstr() function argument
 */

#include <string.h>

void find_substring(const char *haystack) {
    char *result = strstr(haystack,  // Line 10 - VIOLATION
    #ifdef PATTERN_A
        "pattern_a"
    #else
        "pattern_b"
    #endif
    );
}

int main(void) {
    find_substring("test string");
    return 0;
}
