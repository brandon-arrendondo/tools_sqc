/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Increment in assert (may not evaluate when NDEBUG defined)
 */

#include <assert.h>
#include <stddef.h>

void process(size_t index) {
    // Side effect in assert - may not execute
    assert(index++ > 0);  // Line 12 - VIOLATION
    // If NDEBUG is defined, increment doesn't happen
}

int main(void) {
    process(5);
    return 0;
}
