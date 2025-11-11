/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in realloc() function argument
 */

#include <stdlib.h>

void resize_buffer(void *ptr) {
    void *new_ptr = realloc(ptr,  // Line 10 - VIOLATION
    #ifdef DOUBLE_SIZE
        2048
    #else
        1024
    #endif
    );
}

int main(void) {
    return 0;
}
