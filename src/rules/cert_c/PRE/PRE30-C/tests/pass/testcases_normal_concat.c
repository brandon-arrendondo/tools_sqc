/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: Concatenation that doesn't create UCN syntax
 */

// Concatenation not forming UCN - COMPLIANT
#define MAKE_VAR(prefix, suffix) prefix##suffix

void normal_concat_test(void) {
    int var123;

    // Concatenates to "var123", not a UCN - COMPLIANT
    MAKE_VAR(var, 123) = 50;
}

int main(void) {
    normal_concat_test();
    return 0;
}
