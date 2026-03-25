/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Division/modulo with proper guards
 */

/* Guarded division */
int safe_divide(int a, int b) {
    if (b == 0) return 0;
    return a / b;
}

/* Guarded modulo */
int safe_modulo(int a, int b) {
    if (b == 0) return 0;
    return a % b;
}

/* Division by constant — always safe */
int divide_by_constant(int a) {
    return a / 2;
}
