/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in sprintf() function argument
 */

#include <stdio.h>

void format_string(char *buffer, int value) {
    sprintf(buffer, "Value: %d",  // Line 10 - VIOLATION
    #ifdef VERBOSE
        value * 10
    #else
        value
    #endif
    );
}

int main(void) {
    char buf[100];
    format_string(buf, 5);
    return 0;
}
