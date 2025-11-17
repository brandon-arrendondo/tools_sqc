/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: UCN used directly as array name
 */

void array_ucn_test(void) {
    // Array with UCN name - COMPLIANT
    int \u0430[5] = {1, 2, 3, 4, 5};  // Cyrillic small letter a

    // Direct array access - COMPLIANT
    int val = \u0430[2];
    \u0430[3] = 10;
}

int main(void) {
    array_ucn_test();
    return 0;
}
