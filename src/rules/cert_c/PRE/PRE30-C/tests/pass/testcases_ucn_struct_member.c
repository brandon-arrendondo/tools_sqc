/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: UCN used as struct member name directly
 */

struct data {
    int \u0440;  // Cyrillic small letter er - COMPLIANT
    int \u0441;  // Cyrillic small letter es
};

void struct_ucn_test(void) {
    struct data d;

    // Direct access to UCN members - COMPLIANT
    d.\u0440 = 100;
    d.\u0441 = 200;
}

int main(void) {
    struct_ucn_test();
    return 0;
}
