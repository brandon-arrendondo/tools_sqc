/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Division/modulo with proper checks
 */

/* Division by constant */
int div_by_constant(int a) {
    return a / 2;
}

/* Modulo by constant */
int mod_by_constant(int a) {
    return a % 10;
}

/* Division with zero check */
int checked_div(int a, int b) {
    if (b == 0) {
        return 0;
    }
    return a / b;
}

/* No division */
int no_div(int a, int b) {
    return a + b;
}
