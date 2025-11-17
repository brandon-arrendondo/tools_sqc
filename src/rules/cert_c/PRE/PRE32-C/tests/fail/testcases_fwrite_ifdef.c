/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in fwrite() function argument
 */

#include <stdio.h>

void write_data(FILE *fp, const char *data) {
    fwrite(data, 1,  // Line 10 - VIOLATION
    #ifdef LARGE_WRITE
        2048
    #else
        512
    #endif
    , fp);
}

int main(void) {
    return 0;
}
