/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in while condition through concatenation
 */

#define WHILE_COND(v1, v2) while(v1##v2 > 0)  // Line 7 - VIOLATION

void while_test(void) {
    int \u0510 = 5;  // Cyrillic capital letter reversed ze

    // Creates \u0510 through concatenation
    WHILE_COND(\u05, 10) {  // Line 13 - VIOLATION
        \u0510--;
    }
}

int main(void) {
    while_test();
    return 0;
}
