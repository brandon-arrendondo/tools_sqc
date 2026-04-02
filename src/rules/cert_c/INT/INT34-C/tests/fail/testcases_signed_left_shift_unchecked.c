/*
 * Rule: INT34-C
 * Status: FAIL - Signed left shift without validation
 */

void f(int x, int shift_amt) {
    int result = x << shift_amt;  /* VIOLATION: signed shift, no validation */
}
