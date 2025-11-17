/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in array initialization through concatenation
 */

#define INIT_ARRAY(name1, name2) int name1##name2[] = {1, 2, 3}  // Line 7 - VIOLATION

void init_test(void) {
    // Creates \u0570 through concatenation
    INIT_ARRAY(\u05, 70);  // Line 12 - VIOLATION

    int val = \u0570[0];  // Armenian small letter ho
}

int main(void) {
    init_test();
    return 0;
}
