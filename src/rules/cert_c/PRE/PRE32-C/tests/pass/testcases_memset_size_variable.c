/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Size determined before memset() call
 */

#include <string.h>

void clear_buffer(char *buffer) {
    // Compliant: size determined before function call
#ifdef LARGE_BUFFER
    size_t size = 1024;
#else
    size_t size = 256;
#endif
    memset(buffer, 0, size);
}

int main(void) {
    char buf[1024];
    clear_buffer(buf);
    return 0;
}
