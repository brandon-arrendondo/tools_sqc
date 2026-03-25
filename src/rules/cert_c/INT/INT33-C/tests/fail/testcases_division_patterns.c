/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Division/modulo without zero-check
 */

/* Unguarded division */
int unsafe_divide(int a, int b) {
    return a / b;
}

/* Unguarded modulo */
int unsafe_modulo(int a, int b) {
    return a % b;
}

/* INT_MIN / -1 overflow not checked */
int unsafe_min_divide(int a, int b) {
    return a / b;
}
