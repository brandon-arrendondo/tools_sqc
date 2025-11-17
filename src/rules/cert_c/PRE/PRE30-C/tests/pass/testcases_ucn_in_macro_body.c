/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: UCN defined in macro body without concatenation
 */

// Complete UCN in macro body - COMPLIANT
#define GREEK_ALPHA \u03B1

void macro_body_test(void) {
    int GREEK_ALPHA = 15;
    int result = GREEK_ALPHA * 2;
}

int main(void) {
    macro_body_test();
    return 0;
}
