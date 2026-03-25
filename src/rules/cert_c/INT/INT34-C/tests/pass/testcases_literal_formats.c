/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Various literal formats as shift amounts
 */

/* Hex literal */
unsigned int shift_by_hex(unsigned int x) {
    return x << 0x04;
}

/* Octal literal */
unsigned int shift_by_octal(unsigned int x) {
    return x << 010;
}

/* Suffix literal */
unsigned long shift_by_suffix(unsigned long x) {
    return x << 8u;
}
