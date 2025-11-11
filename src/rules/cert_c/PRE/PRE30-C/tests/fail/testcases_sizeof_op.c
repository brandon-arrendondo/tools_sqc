/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in sizeof operation through concatenation
 */

#define GET_SIZE(t1, t2) sizeof(t1##t2)  // Line 7 - VIOLATION

void sizeof_test(void) {
    int \u04B0;  // Cyrillic capital letter straight u with stroke

    // Creates \u04B0 through concatenation
    size_t size = GET_SIZE(\u04, B0);  // Line 13 - VIOLATION
}

int main(void) {
    sizeof_test();
    return 0;
}
