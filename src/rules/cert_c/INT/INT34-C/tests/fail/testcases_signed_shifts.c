/*
 * Rule: INT34-C
 * Source: testcases
 * Status: FAIL - Signed type shift operations without validation
 */

/* Signed left shift without validation */
int signed_left_shift(int x, int amount) {
    return x << amount;
}

/* Signed right shift without validation */
int signed_right_shift(int x, int amount) {
    return x >> amount;
}

/* Long signed shift */
long long_shift(long x, long amount) {
    return x << amount;
}

/* Short signed shift */
short short_shift(short x, short amount) {
    return x << amount;
}
