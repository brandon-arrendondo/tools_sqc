/*
 * Rule: INT16-C
 * Source: testcases
 * Status: PASS - Unsigned integers used for bitwise operations
 */

/* Unsigned int with bitwise AND is compliant */
void unsigned_bitwise_and(void) {
    unsigned int uval1 = 42;
    unsigned int uresult1 = uval1 & 0xFF;
    (void)uresult1;
}

/* Unsigned int with bitwise OR is compliant */
void unsigned_bitwise_or(void) {
    unsigned int uflags = 0;
    uflags = uflags | 0x01;
    (void)uflags;
}

/* Unsigned int with left shift is compliant */
void unsigned_left_shift(void) {
    unsigned int uval2 = 1;
    unsigned int ushifted = uval2 << 4;
    (void)ushifted;
}

/* Signed int with arithmetic (not bitwise) is compliant */
void signed_arithmetic(void) {
    int sa = 10;
    int sb = sa + 5;
    int sc = sa * 2;
    (void)sb;
    (void)sc;
}

/* Using modulo instead of bitwise AND for odd check */
void modulo_odd_check(void) {
    int mval = 42;
    if (mval % 2 != 0) {
        /* mval is odd */
    }
}
