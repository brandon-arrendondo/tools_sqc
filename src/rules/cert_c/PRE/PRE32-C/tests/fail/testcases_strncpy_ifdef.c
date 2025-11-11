/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in strncpy() function argument
 */

#include <string.h>

void copy_string(char *dest, const char *src) {
    strncpy(dest, src,  // Line 10 - VIOLATION
    #ifdef LARGE_BUFFER
        256
    #else
        64
    #endif
    );
}

int main(void) {
    char dest[256];
    copy_string(dest, "test");
    return 0;
}
