/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in strncat() function argument
 */

#include <string.h>

void append_string(char *dest, const char *src) {
    strncat(dest, src,  // Line 10 - VIOLATION
    #ifdef LONG_APPEND
        128
    #else
        32
    #endif
    );
}

int main(void) {
    char dest[256] = "prefix";
    append_string(dest, "suffix");
    return 0;
}
