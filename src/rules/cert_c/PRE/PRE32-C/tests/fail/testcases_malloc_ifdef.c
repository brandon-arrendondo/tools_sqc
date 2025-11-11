/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in malloc() function argument
 */

#include <stdlib.h>

void allocate_memory(void) {
    int *ptr = malloc(  // Line 10 - VIOLATION
    #ifdef LARGE_ALLOC
        1024 * sizeof(int)
    #else
        256 * sizeof(int)
    #endif
    );
    free(ptr);
}

int main(void) {
    allocate_memory();
    return 0;
}
