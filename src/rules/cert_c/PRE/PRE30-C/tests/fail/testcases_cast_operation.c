/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in cast operation through concatenation
 */

#define CAST_VAR(t1, t2, v) ((int)(t1##t2))  // Line 7 - VIOLATION

void cast_test(void) {
    double \u04C0 = 3.14;  // Cyrillic letter palochka

    // Creates \u04C0 through concatenation
    int val = CAST_VAR(\u04, C0, \u04C0);  // Line 13 - VIOLATION
}

int main(void) {
    cast_test();
    return 0;
}
