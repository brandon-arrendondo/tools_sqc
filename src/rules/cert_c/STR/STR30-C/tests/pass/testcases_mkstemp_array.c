/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Passing array to mkstemp(), not string literal
 */

#include <stdlib.h>

void func(void) {
    // Compliant: using array, not string literal
    char fname[] = "/tmp/edXXXXXX";
    mkstemp(fname);
}

int main(void) {
    func();
    return 0;
}
