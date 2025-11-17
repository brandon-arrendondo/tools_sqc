/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in compound assignment through concatenation
 */

#define ADD_ASSIGN(v1, v2, val) v1##v2 += val  // Line 7 - VIOLATION

void compound_test(void) {
    int \u0530 = 10;  // Armenian capital letter ayb

    // Creates \u0530 through concatenation
    ADD_ASSIGN(\u05, 30, 5);  // Line 13 - VIOLATION
}

int main(void) {
    compound_test();
    return 0;
}
