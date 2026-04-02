/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Unsigned right shifts detected via parameter type declarations
 */

/* Unsigned type inferred from parameter declaration — right shift safe */
unsigned int right_shift_basic(unsigned int val, unsigned int amt) {
    return val >> amt;
}

/* Multiple unsigned params with natural names */
unsigned int right_shift_mask(unsigned int data, unsigned int shift) {
    return data >> shift;
}

/* Unsigned long variant */
unsigned long right_shift_long(unsigned long value, unsigned int bits) {
    return value >> bits;
}
