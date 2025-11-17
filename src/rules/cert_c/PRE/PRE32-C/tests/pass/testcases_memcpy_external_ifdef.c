/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Preprocessor conditional outside function call
 */

#include <string.h>

void func(const char *src) {
    char *dest;
    // Compliant: conditional outside function call
#ifdef PLATFORM1
    memcpy(dest, src, 12);
#else
    memcpy(dest, src, 24);
#endif
}

int main(void) {
    return 0;
}
