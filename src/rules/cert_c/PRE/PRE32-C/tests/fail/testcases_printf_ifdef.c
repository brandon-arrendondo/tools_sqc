/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: FAIL
 * Reason: #ifdef directive used in printf() function argument
 */

#include <stdio.h>

void print_value(int val) {
    printf("Value: %d\n",  // Line 10 - VIOLATION
    #ifdef DEBUG
        val * 2
    #else
        val
    #endif
    );
}

int main(void) {
    print_value(42);
    return 0;
}
