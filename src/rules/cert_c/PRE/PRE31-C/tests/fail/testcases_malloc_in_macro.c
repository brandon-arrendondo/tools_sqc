/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: malloc (side effect) in unsafe macro
 */

#include <stdlib.h>

#define CHECK_NULL(ptr) ((ptr) != NULL)  /* UNSAFE */

void allocate_memory(void) {
    // malloc has side effect - may be called twice
    if (CHECK_NULL(malloc(100))) {  // Line 13 - VIOLATION
        // Potential memory leak
    }
}

int main(void) {
    allocate_memory();
    return 0;
}
