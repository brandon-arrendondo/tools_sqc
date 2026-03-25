/*
 * Rule: INT34-C
 * Source: testcases
 * Status: FAIL - Various unvalidated shift patterns
 */

/* Shift by unvalidated variable */
unsigned int unvalidated_left_shift(unsigned int x, unsigned int amount) {
    return x << amount;
}

/* Shift by unvalidated signed variable */
long signed_unvalidated_shift(long x, int amount) {
    return x << amount;
}

/* Right-shift by unvalidated signed variable */
int signed_right_shift_unvalidated(int x, int amount) {
    return x >> amount;
}
