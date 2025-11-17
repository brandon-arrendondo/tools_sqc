/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN for function call through concatenation
 */

#define CALL_FUNC(prefix, suffix) prefix##suffix()  // Line 7 - VIOLATION

int \u0500(void) {  // Function with UCN name
    return 42;
}

void test_function(void) {
    // Creates \u0500 through concatenation
    int val = CALL_FUNC(\u05, 00);  // Line 16 - VIOLATION
}

int main(void) {
    test_function();
    return 0;
}
