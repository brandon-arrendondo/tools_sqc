/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in fread() function argument
 */

#include <stdio.h>

void read_file(FILE *fp, char *buffer) {
    fread(buffer, 1,  // Line 10 - VIOLATION
    #ifdef LARGE_READ
        4096
    #else
        1024
    #endif
    , fp);
}

int main(void) {
    return 0;
}
