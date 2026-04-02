/*
 * Rule: INT34-C
 * Status: PASS - Shift amount bounded by modulo operation
 */

unsigned int f(unsigned int mask, unsigned int pos, unsigned int bi) {
    /* pos % 8u is always in [0,7], safe for any integer type */
    return mask >> (pos % 8u);
}

unsigned int g(unsigned int val, unsigned int n) {
    /* n % 32 is always in [0,31], safe for 32-bit type */
    return val << (n % 32);
}

unsigned int h(unsigned int val, unsigned int n) {
    /* Parenthesized modulo */
    return val >> ((n + 1) % 16u);
}
