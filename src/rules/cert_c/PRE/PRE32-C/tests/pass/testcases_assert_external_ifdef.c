/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Preprocessor conditional outside assert() call
 */

#include <assert.h>

#ifdef STRICT
#define THRESHOLD 100
#else
#define THRESHOLD 50
#endif

void check_value(int x) {
    // Compliant: THRESHOLD resolved before assert
    assert(x > THRESHOLD);
}

int main(void) {
    check_value(75);
    return 0;
}
