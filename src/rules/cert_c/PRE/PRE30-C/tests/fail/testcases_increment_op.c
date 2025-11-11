/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in increment operation through concatenation
 */

#define INCREMENT(v1, v2) v1##v2++  // Line 7 - VIOLATION

void increment_test(void) {
    int \u0480 = 5;  // Cyrillic capital letter koppa

    // Creates \u0480 through concatenation
    INCREMENT(\u04, 80);  // Line 13 - VIOLATION
}

int main(void) {
    increment_test();
    return 0;
}
