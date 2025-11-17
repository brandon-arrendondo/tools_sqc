/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Conditional outside sprintf() invocation
 */

#include <stdio.h>

void format_string(char *buffer, int value) {
    // Compliant: conditional outside function call
#ifdef VERBOSE
    sprintf(buffer, "Value: %d", value * 10);
#else
    sprintf(buffer, "Value: %d", value);
#endif
}

int main(void) {
    char buf[100];
    format_string(buf, 5);
    return 0;
}
