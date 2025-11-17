/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in comma expression through concatenation
 */

#define COMMA_EXPR(v1, v2, a, b) (v1##v2 = a, b)  // Line 7 - VIOLATION

void comma_test(void) {
    int \u0560 = 0;  // Armenian small letter hoats ayb

    // Creates \u0560 through concatenation
    int result = COMMA_EXPR(\u05, 60, 5, 10);  // Line 13 - VIOLATION
}

int main(void) {
    comma_test();
    return 0;
}
