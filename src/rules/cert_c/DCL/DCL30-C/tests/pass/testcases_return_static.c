/*
 * Rule: DCL30-C
 * Status: PASS - Returning address of static local (valid)
 */

int *f(void) {
    static int x = 42;
    return &x;  /* Safe: static has program duration */
}
