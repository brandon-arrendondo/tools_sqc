/*
 * Rule: INT34-C
 * Source: testcases
 * Status: FAIL - Signed right shifts without validation
 */

/* Signed int right shift without validation */
int signed_right_no_check(int x, int amount) {
    return x >> amount;
}

/* Long signed right shift */
long long_right_no_check(long x, long amount) {
    return x >> amount;
}

/* Nested expression with signed shift */
int nested_signed_shift(int x, int y, int z) {
    return (x + y) >> z;
}
