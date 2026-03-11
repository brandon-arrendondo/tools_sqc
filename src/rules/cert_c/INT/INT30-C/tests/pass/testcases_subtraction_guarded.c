/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 *
 * Tests: subtraction guarded by comparison of operands.
 * When `a >= b` or `a > b` is checked before `a - b`, underflow is impossible.
 */

#include <stddef.h>

/* Pattern 1: if (a >= b) { result = a - b; } */
unsigned int guarded_gte(unsigned int end, unsigned int start) {
    if (end >= start) {
        return end - start;
    }
    return 0;
}

/* Pattern 2: if (a > b) { result = a - b; } */
unsigned int guarded_gt(unsigned int a, unsigned int b) {
    if (a > b) {
        return a - b;
    }
    return 0;
}

/* Pattern 3: reversed comparison: if (b <= a) */
unsigned int guarded_reverse_lte(unsigned int a, unsigned int b) {
    if (b <= a) {
        return a - b;
    }
    return 0;
}

/* Pattern 4: reversed comparison: if (b < a) */
unsigned int guarded_reverse_lt(unsigned int a, unsigned int b) {
    if (b < a) {
        return a - b;
    }
    return 0;
}

/* Pattern 5: while loop with comparison guard on simple vars */
void loop_guarded(unsigned int total, unsigned int cost) {
    while (total >= cost) {
        total = total - cost;
    }
}

/* Pattern 6: compound condition with && */
unsigned int compound_guard(unsigned int a, unsigned int b) {
    if (a > 0 && a >= b) {
        return a - b;
    }
    return 0;
}

/* Pattern 7: compound subtraction a -= b guarded */
void compound_sub_guarded(unsigned int *total, unsigned int amount) {
    if (*total >= amount) {
        /* Note: *total is not a simple identifier, so this won't match.
         * But this tests that simple identifier cases work. */
    }
}

/* Pattern 8: simple compound assignment */
unsigned int compound_assign_guarded(unsigned int balance, unsigned int withdrawal) {
    if (balance >= withdrawal) {
        balance -= withdrawal;
    }
    return balance;
}

int main(void) {
    return 0;
}
