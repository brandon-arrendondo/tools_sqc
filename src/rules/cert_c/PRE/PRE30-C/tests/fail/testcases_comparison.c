/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in comparison through concatenation
 */

#define COMPARE(a1, a2, b) (a1##a2 > b)  // Line 7 - VIOLATION

void compare_test(void) {
    int \u0490 = 10;  // Cyrillic capital letter ghe with upturn

    // Creates \u0490 through concatenation
    if (COMPARE(\u04, 90, 5)) {  // Line 13 - VIOLATION
        // Do something
    }
}

int main(void) {
    compare_test();
    return 0;
}
