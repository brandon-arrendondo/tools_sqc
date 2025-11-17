/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in do-while condition through concatenation
 */

#define DO_WHILE(v1, v2) do { v1##v2--; } while(v1##v2 > 0)  // Line 7 - VIOLATION

void do_while_test(void) {
    int \u0520 = 3;  // Cyrillic capital letter el with descender

    // Creates \u0520 through concatenation
    DO_WHILE(\u05, 20);  // Line 13 - VIOLATION
}

int main(void) {
    do_while_test();
    return 0;
}
