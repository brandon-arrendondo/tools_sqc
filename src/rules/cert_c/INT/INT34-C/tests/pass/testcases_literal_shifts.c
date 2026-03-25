/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Shifts by various non-negative integer literal formats
 */

/* Hex literal shift amount */
unsigned int shift_hex(unsigned int x) {
    return x << 0x10;
}

/* Octal literal shift amount */
unsigned int shift_octal(unsigned int x) {
    return x << 010;
}

/* Binary literal shift amount */
unsigned int shift_binary(unsigned int x) {
    return x << 0b1010;
}

/* Literal with unsigned suffix */
unsigned int shift_suffix_u(unsigned int x) {
    return x << 8u;
}

/* Literal with UL suffix */
unsigned int shift_suffix_ul(unsigned int x) {
    return x << 16UL;
}

/* Left shift by zero */
int shift_by_zero(int x) {
    return x << 0;
}

/* Right shift by literal */
int right_shift_literal(int x) {
    return x >> 4;
}
