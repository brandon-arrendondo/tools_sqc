/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Size constant defined outside malloc() call
 */

#include <stdlib.h>

#ifdef LARGE_ALLOC
#define ALLOC_SIZE (1024 * sizeof(int))
#else
#define ALLOC_SIZE (256 * sizeof(int))
#endif

void allocate_memory(void) {
    // Compliant: ALLOC_SIZE resolved before malloc
    int *ptr = malloc(ALLOC_SIZE);
    free(ptr);
}

int main(void) {
    allocate_memory();
    return 0;
}
