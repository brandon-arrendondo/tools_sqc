/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in conditional expression through concatenation
 */

#define TERNARY(v1, v2, a, b) ((v1##v2) ? a : b)  // Line 7 - VIOLATION

void ternary_test(void) {
    int \u04D0 = 1;  // Cyrillic capital letter a with breve

    // Creates \u04D0 through concatenation
    int result = TERNARY(\u04, D0, 10, 20);  // Line 13 - VIOLATION
}

int main(void) {
    ternary_test();
    return 0;
}
