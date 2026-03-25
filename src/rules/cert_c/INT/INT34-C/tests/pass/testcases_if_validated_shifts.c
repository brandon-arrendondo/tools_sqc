/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Shifts validated in enclosing if-statement branches
 */

#include <limits.h>

/* Validated in if-else consequence branch */
int validated_in_consequence(int x, int amount) {
    if (amount >= 0 && amount < 32) {
        return x << amount;
    }
    return 0;
}

/* Validated via PRECISION macro in condition */
#define PRECISION(x) (sizeof(x) * CHAR_BIT)
int validated_precision(int x, int amount) {
    if (amount < 0 || amount >= PRECISION(x)) {
        return 0;
    }
    return x << amount;
}

/* Validated via _MAX check */
int validated_max(int x, int amount) {
    if (amount >= INT_MAX) {
        return 0;
    }
    return x << amount;
}

/* Double negative check: amount < 0 early return then amount >= 32 */
int double_check(int x, int amount) {
    if (amount < 0) {
        return -1;
    }
    if (amount >= 32) {
        return -1;
    }
    return x << amount;
}

/* Validated in alternative (else) branch */
int validated_in_alternative(int x, int amount) {
    if (amount < 0 || amount >= 32) {
        return 0;
    } else {
        return x << amount;
    }
}

/* Check with error handling in consequence */
int validated_with_error(int x, int amount) {
    if (amount >= 32) {
        return -1;
    }
    return x << amount;
}

/* Check with exit in consequence */
void validated_with_exit(int x, int amount) {
    if (amount < 0) {
        exit(1);
    }
    int result = x << amount;
    (void)result;
}
