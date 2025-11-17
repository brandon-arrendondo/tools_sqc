/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: mkstemp() modifies its argument, string literal passed
 */

#include <stdlib.h>

void func(void) {
    mkstemp("/tmp/edXXXXXX");  // Line 10 - VIOLATION: string literal modified by function
}

int main(void) {
    func();
    return 0;
}
