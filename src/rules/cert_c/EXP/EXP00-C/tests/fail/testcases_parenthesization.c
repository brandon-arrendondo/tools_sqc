/*
 * Rule: EXP00-C
 * Source: testcases
 * Status: FAIL - Ambiguous operator precedence without parentheses
 */

/* Bitwise AND mixed with comparison */
int ambiguous_bitwise(int a, int b) {
    return a & b == 0;
}

/* Shift mixed with addition */
int ambiguous_shift(int a, int b) {
    return a << 2 + b;
}
