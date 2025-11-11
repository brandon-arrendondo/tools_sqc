/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in fprintf() function argument
 */

#include <stdio.h>

void log_message(FILE *fp, int code) {
    fprintf(fp, "Code: %d\n",  // Line 10 - VIOLATION
    #ifdef VERBOSE
        code + 1000
    #else
        code
    #endif
    );
}

int main(void) {
    log_message(stdout, 42);
    return 0;
}
