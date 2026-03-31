/*
 * Rule: EXP13-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP13-C violation
 * Description: Properly separated comparisons using logical operators
 */

int check_range(int x) {
    if ((0 < x) && (x < 10))
        return 1;
    return 0;
}

int check_equal(int a, int b, int c) {
    if ((a == b) && (b == c))
        return 1;
    return 0;
}

int simple_compare(int a, int b) {
    if (a < b)
        return -1;
    if (a > b)
        return 1;
    return 0;
}
