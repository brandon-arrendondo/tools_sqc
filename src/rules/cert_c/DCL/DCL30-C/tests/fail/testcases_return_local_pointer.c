/*
 * Rule: DCL30-C
 * Status: FAIL - Returning pointer to local variable
 */

int *f(void) {
    int x = 42;
    int *p = &x;
    return p;  /* VIOLATION: returns pointer to local */
}
