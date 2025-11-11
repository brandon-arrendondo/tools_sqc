/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: Constant defined outside function call
 */

#include <stdio.h>

#ifdef DEBUG
#define MULTIPLIER 2
#else
#define MULTIPLIER 1
#endif

void print_value(int val) {
    // Compliant: preprocessor constant resolved before function call
    printf("Value: %d\n", val * MULTIPLIER);
}

int main(void) {
    print_value(42);
    return 0;
}
