/*
 * Rule: EXP40-C
 * Source: testcases
 * Status: PASS - Known limitation: only double-pointer const bypass detected
 * TODO: Move to fail/ when single-level const-to-non-const detection is implemented (see PLAN.md)
 *
 * These are genuine EXP40-C violations (const removed without cast) but the rule
 * currently only detects `const T **ipp = &ip` patterns, not simple `int *p = &const_int`.
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
