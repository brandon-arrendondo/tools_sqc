/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: UCN used directly in typedef
 */

// Typedef with UCN name directly - COMPLIANT
typedef int \u0460;  // Cyrillic capital letter omega

void typedef_ucn_test(void) {
    \u0460 var = 30;
    var = var + 10;
}

int main(void) {
    typedef_ucn_test();
    return 0;
}
