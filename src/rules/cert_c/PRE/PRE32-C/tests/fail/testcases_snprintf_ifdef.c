/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in snprintf() function argument
 */

#include <stdio.h>

void format_buffer(char *buf, size_t size, int val) {
    snprintf(buf, size, "Value: %d",  // Line 10 - VIOLATION
    #ifdef DOUBLE_VALUE
        val * 2
    #else
        val
    #endif
    );
}

int main(void) {
    char buffer[100];
    format_buffer(buffer, sizeof(buffer), 42);
    return 0;
}
