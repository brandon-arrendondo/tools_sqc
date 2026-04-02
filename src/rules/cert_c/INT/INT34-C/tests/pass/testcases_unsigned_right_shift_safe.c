/*
 * Rule: INT34-C
 * Status: PASS - Unsigned right shift detected via parameter type declaration
 */

void f(unsigned int val, unsigned int shift) {
    /* unsigned int detected from AST; right-shift on unsigned is safe */
    unsigned int result = val >> shift;
}
