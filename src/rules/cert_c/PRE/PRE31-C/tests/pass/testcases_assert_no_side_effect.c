/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: No side effects in assert, increment separate
 */

#include <assert.h>
#include <stddef.h>

void process(size_t index) {
    // No side effect in assert - COMPLIANT
    assert(index > 0);

    // Side effect after assert
    ++index;
}

int main(void) {
    process(5);
    return 0;
}
