/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: Using UCN directly without macros
 */

void direct_usage(void) {
    // Direct UCN usage - COMPLIANT
    int \u00E9 = 10;  // Latin small letter e with acute
    \u00E9 = 20;
    int result = \u00E9 + 5;
}

int main(void) {
    direct_usage();
    return 0;
}
