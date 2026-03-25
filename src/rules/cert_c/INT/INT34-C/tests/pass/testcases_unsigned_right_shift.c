/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Unsigned right shifts with naming convention detection
 */

/* ui_ prefix identifies unsigned — right shift safe */
unsigned int ui_right_shift_basic(unsigned int ui_val, unsigned int ui_amt) {
    return ui_val >> ui_amt;
}

/* u_ prefix identifies unsigned — right shift safe */
unsigned int u_right_shift_basic(unsigned int u_val, unsigned int u_amt) {
    return u_val >> u_amt;
}

/* unsigned_ prefix identifies unsigned — right shift safe */
unsigned int unsigned_prefix_right(unsigned int unsigned_val, unsigned int unsigned_amt) {
    return unsigned_val >> unsigned_amt;
}
