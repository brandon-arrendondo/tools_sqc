/*
 * Rule: EXP40-C
 * Source: testcases
 * Status: FAIL - const-to-non-const pointer assignment without cast
 */

/* Simple const-to-non-const pointer assignment */
void remove_const_simple(void) {
    const int ci = 42;
    int *p = &ci;
    *p = 0;
    (void)p;
}

/* Const pointer parameter to non-const local */
void remove_const_param(const int *cp) {
    int *p = cp;
    *p = 0;
    (void)p;
}
