/*
 * Rule: EXP40-C
 * Source: testcases
 * Status: PASS - Proper const handling
 */

/* Const pointer stays const */
void read_only(const int *cp) {
    int val = *cp;
    (void)val;
}

/* Non-const to non-const — fine */
void non_const_assign(void) {
    int x = 42;
    int *p = &x;
    *p = 0;
    (void)p;
}

/* Const-to-const assignment — fine */
void const_to_const(const int *cp) {
    const int *cp2 = cp;
    (void)cp2;
}
