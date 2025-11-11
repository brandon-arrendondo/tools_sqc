/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: Multiple complete UCNs as separate macro arguments
 */

#define add_vars(var1, var2) (var1 + var2)  // No concatenation - COMPLIANT

void multiple_ucn_test(void) {
    int \u0402 = 10;  // Cyrillic capital letter DJE
    int \u0403 = 20;  // Cyrillic capital letter GJE

    // Complete UCNs as separate arguments - COMPLIANT
    int sum = add_vars(\u0402, \u0403);
}

int main(void) {
    multiple_ucn_test();
    return 0;
}
