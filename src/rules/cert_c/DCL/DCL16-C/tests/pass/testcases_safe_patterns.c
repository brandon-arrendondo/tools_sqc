/* Rule: DCL16-C
 * Source: testcases
 * Status: PASS - Uppercase 'L' suffix on integer literals
 */

/* Case 1: Uppercase L suffix */
void test_uppercase_L(void) {
    long x = 100L;
    long y = 0L;
}

/* Case 2: Uppercase LL suffix */
void test_uppercase_LL(void) {
    long long big = 999999LL;
    long long neg = -50LL;
}

/* Case 3: Uppercase L with unsigned suffix */
void test_uppercase_L_unsigned(void) {
    unsigned long val = 42LU;
    unsigned long long big = 123LLU;
}

/* Case 4: No long suffix at all (plain integers) */
void test_no_suffix(void) {
    int a = 100;
    int b = 0xFF;
    int c = 42;
}
