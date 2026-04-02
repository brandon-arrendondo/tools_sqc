/*
 * Rule: INT34-C
 * Status: PASS - Unsigned right shift with naming convention hint
 */

void f(unsigned int ui_val, unsigned int ui_shift) {
    /* ui_ prefix signals unsigned; right-shift on unsigned is safe */
    unsigned int result = ui_val >> ui_shift;
}
