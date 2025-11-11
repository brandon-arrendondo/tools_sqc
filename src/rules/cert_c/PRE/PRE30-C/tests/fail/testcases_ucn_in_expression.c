/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in arithmetic expression through concatenation
 */

#define ADD(var1, var2, val) (var1##var2 + val)  // Line 7 - VIOLATION

void expression_test(void) {
    int \u0402 = 10;  // Cyrillic capital letter DJE

    // Creates \u0402 through concatenation in expression
    int result = ADD(\u04, 02, 5);  // Line 13 - VIOLATION
}

int main(void) {
    expression_test();
    return 0;
}
