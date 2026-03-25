/*
 * Rule: INT00-C
 * Source: testcases
 * Status: PASS - Known limitation: unsigned wrap/mixed comparison not detected
 * TODO: Move to fail/ when implemented (see PLAN.md)
 */

/* Unsigned wrap on subtraction */
void unsigned_subtract(unsigned int a, unsigned int b) {
    unsigned int result = a - b;
    (void)result;
}

/* Mixed signed/unsigned comparison */
void mixed_comparison(int a, unsigned int b) {
    if (a < b) {
        return;
    }
}
