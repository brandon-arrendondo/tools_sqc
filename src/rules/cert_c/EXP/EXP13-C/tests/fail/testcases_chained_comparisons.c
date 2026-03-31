/*
 * Rule: EXP13-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP13-C violation
 * Description: Chained relational and equality operators
 */

int check_range(int x) {
    if (0 < x < 10)     /* Violation: chained < */
        return 1;
    return 0;
}

int check_equal(int a, int b, int c) {
    if (a == b == c)     /* Violation: chained == */
        return 1;
    return 0;
}

int check_mixed(int a, int b, int c) {
    if (a <= b <= c)     /* Violation: chained <= */
        return 1;
    return 0;
}
