/*
 * Rule: INT34-C
 * Source: testcases
 * Status: FAIL - Complex expressions as shift amounts
 */

/* Shift by expression result */
unsigned int shift_by_expression(unsigned int x, unsigned int a, unsigned int b) {
    return x << (a + b);
}

/* Shift by function return value */
unsigned int get_amount(void);
unsigned int shift_by_function(unsigned int x) {
    return x << get_amount();
}

/* Shift in ternary without validation */
unsigned int shift_ternary(unsigned int x, unsigned int a, int flag) {
    return x << (flag ? a : 16);
}
