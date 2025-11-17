/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN through nested macro concatenation
 */

#define PASTE(a, b) a##b  // Line 7 - VIOLATION
#define CREATE_VAR(x, y) int PASTE(x, y)  // Line 8 - Uses PASTE

void nested_test(void) {
    // Creates \u0470 through nested concatenation
    CREATE_VAR(\u04, 70);  // Line 13 - VIOLATION
    \u0470 = 40;
}

int main(void) {
    nested_test();
    return 0;
}
