/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in calloc() function argument
 */

#include <stdlib.h>

void allocate_array(void) {
    int *arr = calloc(  // Line 10 - VIOLATION
    #ifdef LARGE_ARRAY
        1000
    #else
        100
    #endif
    , sizeof(int));
    free(arr);
}

int main(void) {
    allocate_array();
    return 0;
}
