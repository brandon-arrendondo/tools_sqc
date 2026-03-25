/* Rule: DCL16-C
 * Source: testcases
 * Status: FAIL - Lowercase 'l' suffix on integer literals
 */

/* Case 1: Simple lowercase l suffix */
void test_lowercase_l(void) {
    long x = 100l;
    long y = 0l;
}

/* Case 2: Lowercase ll suffix */
void test_lowercase_ll(void) {
    long long big = 999999ll;
    long long neg = -50ll;
}

/* Case 3: Lowercase l with unsigned suffix */
void test_lowercase_l_unsigned(void) {
    unsigned long val = 42lu;
    unsigned long long big = 123llu;
}

/* Case 4: Hex literal with lowercase l */
void test_hex_lowercase_l(void) {
    long hex_val = 0xFFl;
    long long hex_big = 0xABCDll;
}
