/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Concatenating to form long UCN \Unnnnnnnn
 */

#define CONCAT(a, b) a##b  // Line 7 - VIOLATION

void test_long_ucn(void) {
    int \U00010348;  // Gothic letter hwair

    // Creates \U00010348 through concatenation
    CONCAT(\U000103, 48) = 5;  // Line 13 - VIOLATION
}

int main(void) {
    test_long_ucn();
    return 0;
}
