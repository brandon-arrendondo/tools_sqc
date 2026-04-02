/*
 * Rule: INT34-C
 * Status: PASS - Shift by hex literal is compile-time checkable
 */

unsigned int f(unsigned int val) {
    return val << 0x10;  /* 16 in hex - literal shift amount */
}

unsigned int g(unsigned int val) {
    return val >> 0b0100;  /* 4 in binary - literal shift amount */
}

unsigned int h(unsigned int val) {
    return val << 010;  /* 8 in octal - literal shift amount */
}
