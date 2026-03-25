/*
 * Rule: EXP00-C
 * Source: testcases
 * Status: PASS - Clear precedence with parentheses
 */

/* Parenthesized bitwise AND */
int clear_bitwise(int a, int b) {
    return (a & b) == 0;
}

/* Parenthesized shift */
int clear_shift(int a, int b) {
    return (a << 2) + b;
}

/* Same-precedence operations — no ambiguity */
int same_precedence(int a, int b, int c) {
    return a + b + c;
}
