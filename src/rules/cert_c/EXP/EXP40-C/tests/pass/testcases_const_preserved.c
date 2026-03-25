/*
 * Rule: EXP40-C
 * Source: testcases
 * Status: PASS - Const qualification properly preserved
 */

/* Const pointer to const variable */
void const_preserved(void) {
    const int ci = 42;
    const int *p = &ci;
    (void)p;
}

/* Non-const pointer to non-const variable */
void nonconst_to_nonconst(void) {
    int x = 42;
    int *p = &x;
    *p = 10;
}

/* Const char* from string literal */
void string_literal_const(void) {
    const char *s = "hello";
    (void)s;
}
