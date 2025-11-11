/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in for loop through concatenation
 */

#define FOR_LOOP(v1, v2, n) for(v1##v2 = 0; v1##v2 < n; v1##v2++)  // Line 7 - VIOLATION

void loop_test(void) {
    int \u0500;  // Cyrillic capital letter komi de

    // Creates \u0500 through concatenation
    FOR_LOOP(\u05, 00, 10) {  // Line 13 - VIOLATION
        // Loop body
    }
}

int main(void) {
    loop_test();
    return 0;
}
