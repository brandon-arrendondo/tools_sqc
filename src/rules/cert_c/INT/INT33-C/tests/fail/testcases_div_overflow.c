/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Division/modulo without checking for zero
 */

/* Unchecked division */
int unchecked_div(int a, int b) {
    return a / b;
}

/* Unchecked modulo */
int unchecked_mod(int a, int b) {
    return a % b;
}

/* Division with variable divisor */
long unchecked_long_div(long a, long b) {
    return a / b;
}

/* Modulo in expression */
int mod_in_expr(int x, int n) {
    int result = x % n + 1;
    return result;
}
