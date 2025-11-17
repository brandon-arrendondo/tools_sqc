/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in return statement through concatenation
 */

#define RETURN_VAR(v1, v2) return v1##v2  // Line 7 - VIOLATION

int get_value(void) {
    int \u04E0 = 55;  // Cyrillic capital letter abkhasian dze

    // Creates \u04E0 through concatenation
    RETURN_VAR(\u04, E0);  // Line 13 - VIOLATION
}

int main(void) {
    return get_value();
}
