/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: Long form UCN used directly without concatenation
 */

#define SET_VALUE(var, val) var = val  // No concatenation - COMPLIANT

void long_ucn_test(void) {
    // Long form UCN (8 digits) - COMPLIANT
    int \U00010348;  // Gothic letter hwair

    // Complete UCN as argument - COMPLIANT
    SET_VALUE(\U00010348, 42);
}

int main(void) {
    long_ucn_test();
    return 0;
}
