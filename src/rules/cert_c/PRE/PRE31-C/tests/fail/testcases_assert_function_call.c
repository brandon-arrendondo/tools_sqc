/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Function call with side effects in assert
 */

#include <assert.h>

int counter = 0;

int increment_counter(void) {
    return ++counter;  // Has side effect
}

void validate(void) {
    // Function with side effect in assert
    assert(increment_counter() > 0);  // Line 17 - VIOLATION
}

int main(void) {
    validate();
    return 0;
}
