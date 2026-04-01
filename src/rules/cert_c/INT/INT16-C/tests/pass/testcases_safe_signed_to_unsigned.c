/*
 * Rule: INT16-C
 * Source: testcases
 * Status: PASS - Safe signed-to-unsigned conversions with range checks
 */

/* Guarded by >= 0 check */
unsigned int safe_convert(int x) {
    if (x >= 0) {
        return x;
    }
    return 0;
}

/* Guarded assignment */
void safe_assign(int val) {
    unsigned int uval;
    if (val >= 0) {
        uval = val;
        (void)uval;
    }
}
