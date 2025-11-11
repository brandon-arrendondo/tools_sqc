/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in logical AND operation through concatenation
 */

#define LOGICAL_AND(v1, v2, v3) (v1##v2 && v3)  // Line 7 - VIOLATION

void logical_test(void) {
    int \u0540 = 1;  // Armenian capital letter ho
    int other = 1;

    // Creates \u0540 through concatenation
    if (LOGICAL_AND(\u05, 40, other)) {  // Line 15 - VIOLATION
        // Do something
    }
}

int main(void) {
    logical_test();
    return 0;
}
