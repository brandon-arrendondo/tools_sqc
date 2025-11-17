/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in multiple variable declaration through concatenation
 */

#define DECLARE_MULTI(v1, v2) int v1##v2, other  // Line 7 - VIOLATION

void multi_decl_test(void) {
    // Creates \u0580 through concatenation
    DECLARE_MULTI(\u05, 80);  // Line 12 - VIOLATION

    \u0580 = 10;  // Armenian small letter ra
    other = 20;
}

int main(void) {
    multi_decl_test();
    return 0;
}
