/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in memset() function argument
 */

#include <string.h>

void clear_buffer(char *buffer) {
    memset(buffer, 0,  // Line 10 - VIOLATION
    #ifdef LARGE_BUFFER
        1024
    #else
        256
    #endif
    );
}

int main(void) {
    char buf[1024];
    clear_buffer(buf);
    return 0;
}
