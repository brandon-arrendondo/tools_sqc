/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Concatenating to form short UCN \unnnn
 */

#define MAKE_ID(prefix, suffix) prefix##suffix  // Line 7 - VIOLATION

void test_short_ucn(void) {
    int \u00E9;  // Valid UCN identifier

    // Creates \u00E9 through concatenation
    MAKE_ID(\u00, E9) = 10;  // Line 13 - VIOLATION
}

int main(void) {
    test_short_ucn();
    return 0;
}
