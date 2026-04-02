/*
 * Rule: DCL30-C
 * Status: PASS - Returning pointer parameter (valid)
 */

int *f(int *input) {
    return input;  /* Safe: not a local variable */
}
