/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Shift amounts validated via range extraction from conditions
 */

/* Less-than condition with early return */
int lt_validated(int x, int amount) {
    if (amount >= 32) {
        return 0;
    }
    return x << amount;
}

/* Compound condition: >= 0 AND < 32 inside if */
int compound_and_validated(int x, int amount) {
    if (amount >= 0 && amount < 32) {
        return x << amount;
    }
    return 0;
}

/* Negative check with early return */
int negative_check(int x, int amount) {
    if (amount < 0) {
        return 0;
    }
    return x << amount;
}
