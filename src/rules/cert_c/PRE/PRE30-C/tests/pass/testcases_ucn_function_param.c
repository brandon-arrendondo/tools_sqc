/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: UCN used as function parameter name directly
 */

// Function with UCN parameter - COMPLIANT
int process(\u0410 int) {  // Cyrillic capital letter A
    return \u0410 * 2;
}

void ucn_param_test(void) {
    int result = process(5);
}

int main(void) {
    ucn_param_test();
    return 0;
}
