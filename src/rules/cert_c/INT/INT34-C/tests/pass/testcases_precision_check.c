/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Shift amounts validated via PRECISION/MAX macros
 */

#define PRECISION(x) (sizeof(x) * 8)

/* Validated with PRECISION macro */
unsigned int shift_with_precision(unsigned int x, unsigned int amount) {
    if (amount >= PRECISION(x)) {
        return 0;
    }
    return x << amount;
}

/* Validated via _MAX constant reference */
unsigned int shift_with_max(unsigned int x, int amount) {
    if (amount >= UINT_MAX) {
        return 0;
    }
    return x << amount;
}

/* Comparison child node with 0 on RHS */
int shift_with_zero_cmp(int x, int amount) {
    if (amount < 0 || amount >= 32) {
        return 0;
    }
    return x << amount;
}

/* Comparison child node with 0 on LHS */
int shift_with_zero_lhs(int x, int amount) {
    if (0 > amount) {
        return 0;
    }
    return x << amount;
}
