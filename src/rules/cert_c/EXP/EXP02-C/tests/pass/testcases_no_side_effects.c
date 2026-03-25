/*
 * Rule: EXP02-C
 * Source: testcases
 * Status: PASS - No side effects in logical operator operands
 */

/* Pure comparisons — no side effects */
int pure_logic(int a, int b, int c) {
    if (a > 0 && b > 0 && c > 0) {
        return 1;
    }
    if (a == 0 || b == 0) {
        return 0;
    }
    return -1;
}

/* Null check + dereference — idiomatic, no side effect */
int safe_deref(int *p) {
    if (p != NULL && *p > 0) {
        return *p;
    }
    return 0;
}
