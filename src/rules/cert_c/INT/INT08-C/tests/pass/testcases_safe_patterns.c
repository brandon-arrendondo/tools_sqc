/* Rule: INT08-C
 * Source: testcases
 * Status: PASS - Non-narrow types or no arithmetic
 */

/* Case 1: int arithmetic — not a narrow type */
void test_int_arith(void) {
    int x = 200;
    int y = 2;
    int result = x * y;
    (void)result;
}

/* Case 2: long arithmetic — not a narrow type */
void test_long_arith(void) {
    long a = 32000;
    long b = 1000;
    long result = a + b;
    (void)result;
}

/* Case 3: No arithmetic at all */
void test_no_arith(void) {
    int x = 42;
    (void)x;
}
