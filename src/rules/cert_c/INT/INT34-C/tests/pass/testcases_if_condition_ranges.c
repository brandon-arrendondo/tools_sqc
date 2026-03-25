/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Shift amounts validated by enclosing if-conditions
 */

#include <limits.h>

/* Validated via < upper bound with early return */
unsigned int validated_lt_return(unsigned int x, unsigned int amount) {
    if (amount >= 32) {
        return 0;
    }
    return x << amount;
}

/* Validated via CHAR_BIT in condition text */
unsigned int validated_char_bit(unsigned int x, int amount) {
    if (amount < 0 || amount >= (int)(sizeof(unsigned int) * CHAR_BIT)) {
        return 0;
    }
    return x << amount;
}

/* Validated via negative check in condition text */
int validated_negative_check(int x, int amount) {
    if (amount < 0) {
        return 0;
    }
    if (amount >= 32) {
        return 0;
    }
    return x << amount;
}

/* Validated via compound && condition */
unsigned int validated_compound_and(unsigned int x, unsigned int amount) {
    if (amount >= 0 && amount < 32) {
        return x << amount;
    }
    return 0;
}

/* Validated via 0 > var pattern */
int validated_zero_gt(int x, int amount) {
    if (0 > amount) {
        return 0;
    }
    return x << amount;
}

/* Validated via 0 <= var pattern */
int validated_zero_le(int x, int amount) {
    if (0 <= amount) {
        return x << amount;
    }
    return 0;
}
