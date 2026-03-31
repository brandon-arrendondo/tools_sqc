/*
 * Rule: FLP36-C
 * Source: testcases
 * Status: FAIL - Should trigger FLP36-C violation
 *
 * Assignment of long to float loses precision
 */

void long_to_float_implicit(void) {
    long big = 1234567890L;
    /* VIOLATION: float only has ~7 decimal digits of precision */
    float approx = big;
}
