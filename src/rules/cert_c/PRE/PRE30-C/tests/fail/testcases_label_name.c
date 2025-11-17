/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in label name through concatenation
 */

#define MAKE_LABEL(l1, l2) l1##l2:  // Line 7 - VIOLATION

void label_test(void) {
    // Creates \u0590 through concatenation
    MAKE_LABEL(\u05, 90)  // Line 12 - VIOLATION
        int x = 10;

    goto \u0590;  // Hebrew accent etnahta
}

int main(void) {
    label_test();
    return 0;
}
