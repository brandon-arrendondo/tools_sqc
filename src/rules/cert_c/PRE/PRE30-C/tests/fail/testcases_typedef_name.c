/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in typedef through concatenation
 */

#define MAKE_TYPEDEF(t1, t2) typedef int t1##t2  // Line 7 - VIOLATION

// Creates typedef with UCN \u0460 through concatenation
MAKE_TYPEDEF(\u04, 60);  // Line 11 - VIOLATION

void typedef_test(void) {
    \u0460 var = 30;  // Use the typedef
}

int main(void) {
    typedef_test();
    return 0;
}
